#include "thread.h"

__BEGIN_API

Thread* Thread::_running = 0;  //inicializa _running
int Thread::_countID = 0;      //inicializa contador de id

Thread Thread::_main;
Thread Thread::_dispatcher;
Ordered_List<Thread> Thread::_ready;
CPU::Context Thread::_main_context;
Ordered_List<Thread> Thread::_suspended;

Timer* Thread::_timer = 0;

int Thread::switch_context(Thread * prev, Thread * next) {
    db<Thread>(TRC) << "TRC: Switch context\n";

    if(prev == next || prev == 0 || next == 0){
        db<Thread>(ERR) << "ERR: Entradas inválidas\n";
        return -1;
    }
    
    _running = next;        //_running aponta pra próxima thread
    next->_state = RUNNING; //thread fica no estado RUNNING
    _ready.remove(next);    //remove thread da fila

    db<Thread>(TRC) << "TRC: Thread " << next->_id << " removida da fila, vai executar\n";

    //if(_ready.empty()) _running = 0;

    db<Thread>(TRC) << "RUNNING switch_context = " << _running->_id << "\n";

    if ( _timer != 0){
        _timer->reset();
    }

    CPU::switch_context(prev->context(),next->context());

    return 0;
}

void Thread::thread_exit(int exit_code)
{
    db<Thread>(TRC) << "TRC: Thread_exit exit_code = " << exit_code << "\n";

    db<Thread>(TRC) << "RUNNING thread_exit = " << _running->_id << "\n";

    _state = FINISHING; //thread fica no estado FINISHING
    // Implementação trabalho 4
    _exit_code = exit_code;

    if ( !_suspended.empty()){
        _suspend = _suspended.head()->object();
        _suspend->resume();
    }
    
    yield();
}

int Thread::id() {
    return _id;
}

CPU::Context* Thread::context() const {
    return _context;
}

void Thread::dispatcher() {
    db<Thread>(TRC) << "TRC: Thread dispatcher\n";

    Thread *prox = 0; //próxima thread

    //enquanto há thread de usuário
    while(!_ready.empty()) {
        db<Thread>(TRC) << "TRC: Início loop dispatcher\n";

        prox = _ready.head()->object(); //próxima thread é a cabeça da fila
        _dispatcher._state = READY;     //dispatcher fica no estado READY
        _ready.insert_head(&_dispatcher._link); //reinsere dispatcher na cabeça da fila

        switch_context(&_dispatcher,prox);

        db<Thread>(TRC) << "TRC: voltou dispatcher, thread -> " << prox->_id << "\n";

        //se thread está FINISHING, já foi removida da fila (na switch_context), não faz nada
        if(prox->_state == FINISHING) db<Thread>(TRC) << "TRC: thread atual finalizando\n";
    }

    //acabou threads de usuário
    db<Thread>(INF) << "INF: dispatcher finalizando\n";

    _dispatcher._state = FINISHING; //dispatcher fica no estado FINISHING

    switch_context(&_dispatcher,&_main);
}

void Thread::init(void (*main)(void *))
{
    db<Thread>(TRC) << "TRC: Thread::init\n";
    std::string main_name = "main";

    //placement new
    new (&_main) Thread(main,(void *)main_name.data());
    new (&_main_context) CPU::Context(main,(void *)main_name.data());

    //Trabalho 6
    if(Traits<Timer>::active_timer){
        Thread::_timer = new Timer(Traits<Timer>::quantum, (void(*)(int))reschedule); //precisa de casting pra int
    } 
    else new (&_dispatcher) Thread(dispatcher);

    _running = &_main;
    _main._state = RUNNING;
    _ready.remove(&_main);

    db<Thread>(TRC) << "RUNNING init = " << _running->_id << "\n";

    CPU::switch_context(&_main_context,_main.context());
}

void Thread::yield() {
    db<Thread>(TRC) << "TRC: Thread yield\n";

    db<Thread>(TRC) << "RUNNING yield= " << _running->_id << "\n";

    Thread *prox = 0; //próxima thread
    Thread *atual = 0; //thread atual

    // Exceção
    if (_ready.empty()){
        db<Thread>(INF) << "INF: yield: _ready vazia!!\n";
	if(_running == &_main) return;
        CPU::switch_context(_running->_context,_main._context);
    } 

    prox = _ready.head()->object(); //próxima thread é a cabeça da fila
    atual = _running;               //atual é a running

    if(atual == &_main){
        db<Thread>(INF) << "INF: Thread MAIN\n";
        atual->_state = READY; //será reinserida na fila apenas após as outras threads finalizarem
    }
    else if(atual->_state == FINISHING) db<Thread>(INF) << "INF: Thread atual finalizando\n"; //não reinsere na fila
    else if(atual->_state == WAITING) db<Thread>(INF) << "INF: Thread atual waiting\n"; //já está na fila de _waiting
    else {
        atual->_link.rank(std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::high_resolution_clock::now().time_since_epoch()).count()); //atualiza prioridade
        atual->_state = READY;        //thread atual fica no estado READY
        _ready.insert(&atual->_link); //reinsere thread na fila
    }

    db<Thread>(TRC) << "TRC: prox da yield = " << prox->_id << "\n";
    switch_context(atual,prox);
}

Thread::~Thread() {
    db<Thread>(TRC) << "TRC: destrutor da thread -> " << _id << "\n";

    db<Thread>(TRC) << "RUNNING destrutor = " << _running->_id << "\n";

    //se a thread pertence a uma fila de waiting de um semáforo
    if(_wait != 0) _wait->remove(this); //remove thread da fila de waiting do semáforo

    delete _context; //desaloca context
    _context = nullptr;

    if(this == &_main) delete _timer;
}

/* =================== */
// Implementação trabalho 4
int Thread::join()
{
    db<Thread>(TRC) << "TRC: thread join() -> " << _id << "\n";
    
    db<Thread>(TRC) << "RUNNING join = " << _running->_id << "\n";

    _running->suspend(); //suspende a thread que está executando
    return _exit_code;
}

void Thread::resume()
{
    db<Thread>(TRC) << "TRC: thread resume() -> " << _id << "\n";

    db<Thread>(TRC) << "RUNNING resume = " << _running->_id << "\n";

    _suspended.remove(this); //remove da fila de _suspended
    _state = READY;
    _ready.insert(&this->_link); //reinsere na fila de _ready    
}

void Thread::suspend()
{
    db<Thread>(TRC) << "TRC: thread suspend() -> " << _id << "\n";

    db<Thread>(TRC) << "RUNNING suspend = " << _running->_id << "\n";

    _state = SUSPENDED;
    _suspended.insert(&this->_link); //insere na fila de _suspended

    yield();
}

//-----------------Trabalho 5-----------------//
void Thread::sleep(Ordered_List<Thread> * _waiting)
{
    db<Thread>(TRC) << "TRC: thread sleep()\n";

    if ( _waiting == nullptr) exit(-1);
    
    Thread *atual = _running; //thread atual
    atual->_state = WAITING;
    _waiting->insert(&atual->_link); //insere na fila de _waiting

    atual->_wait = _waiting; //armazena em qual fila de waiting de qual semáforo a thread está
}

void Thread::wakeup(Ordered_List<Thread> * _waiting)
{
    db<Thread>(TRC) << "TRC: thread wakeup()\n";

    if ( _waiting == nullptr) exit(-1);
    
    if(!_waiting->empty()) //fila de threads _waiting não vazia
    {
        Thread *wake = _waiting->head()->object();
        _waiting->remove(wake); //retira cabeça da fila
        wake->_state = READY;
        _ready.insert(&wake->_link); //reinsere na fila de prontos
    }
}

//-----------------Trabalho 6-----------------//
void Thread::reschedule()
{
    db<Thread>(TRC) << "TRC: reschedule\n";
    db<Thread>(TRC) << "RUNNING reschedule = " << _running->_id << "\n";
    yield();
}

__END_API
