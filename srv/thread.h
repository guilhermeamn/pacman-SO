#ifndef thread_h
#define thread_h

#include "cpu.h"
#include "traits.h"
#include "debug.h"
#include "list.h"
#include <ctime> 
#include <chrono>
#include "timer.h"

__BEGIN_API

class Thread
{
protected:
    typedef CPU::Context Context;
public:

    typedef Ordered_List<Thread> Ready_Queue;

    // Thread State
    enum State {
        RUNNING,
        READY,
        FINISHING,
        SUSPENDED,
        WAITING
    };

    /*
     * Construtor vazio. Necessário para inicialização, mas sem importância para a execução das Threads.
     */ 
    Thread() {
        db<Thread>(TRC) << "TRC: Thread()\n";
     }

    /*
     * Cria uma Thread passando um ponteiro para a função a ser executada
     * e os parâmetros passados para a função, que podem variar.
     * Cria o contexto da Thread.
     * PS: devido ao template, este método deve ser implementado neste mesmo arquivo .h
     */ 
    template<typename ... Tn>
    Thread(void (* entry)(Tn ...), Tn ... an);

    /*
     * Retorna a Thread que está em execução.
     */ 
    static Thread * running() { return _running; }

    /*
     * Método para trocar o contexto entre duas thread, a anterior (prev)
     * e a próxima (next).
     * Deve encapsular a chamada para a troca de contexto realizada pela class CPU.
     * Valor de retorno é negativo se houve erro, ou zero.
     */ 
    static int switch_context(Thread * prev, Thread * next);

    /*
     * Termina a thread.
     * exit_code é o código de término devolvido pela tarefa (ignorar agora, vai ser usado mais tarde).
     * Quando a thread encerra, o controle deve retornar à main. 
     */  
    void thread_exit (int exit_code);

    /*
     * Retorna o ID da thread.
     */ 
    int id();

    /*
     * Retorna o contexto da thread
     */
    Context* context() const;

    /*
     * NOVO MÉTODO DESTE TRABALHO.
     * Daspachante (disptacher) de threads. 
     * Executa enquanto houverem threads do usuário.
     * Chama o escalonador para definir a próxima tarefa a ser executada.
     */
    static void dispatcher();

    /*
     * NOVO MÉTODO DESTE TRABALHO.
     * Realiza a inicialização da class Thread.
     * Cria as Threads main e dispatcher.
     */ 
    static void init(void (*main)(void *));


    /*
     * Devolve o processador para a thread dispatcher que irá escolher outra thread pronta
     * para ser executada.
     */
    static void yield();

    /*
     * Destrutor de uma thread. Realiza todo os procedimentos para manter a consistência da classe.
     */ 
    ~Thread();

    /*
     * Qualquer outro método que você achar necessário para a solução.
     */ 

    // Implementação do trabalho 4
    /*
     * Suspende a thread em execução até que a thread "alvo" finalize.
     */
    int join();

    /*
     * Suspende a thread até que resume() seja chamado.
     */
    void suspend();

    /*
     * Acorda uma thread que estava suspensa
     */
    void resume();

    //---------------------Trabalho 5-------------------//
    /*
     * O método sleep() deve colocar a Thread que não conseguir acessar o semáforo para dormir e
     * mudar seu estado para WAITING
     */
    static void sleep( Ordered_List<Thread> * _waiting);

    /*
     * O método wakeup() deve acordar uma Thread que estava dormindo no
     * semáforo
     */
    static void wakeup(Ordered_List<Thread> * _waiting);

    //---------------------Trabalho 6-------------------//
    /* O metodo reschedule() será chamado através de uma
     * interrupação, configurado na classe timer.
     * Onde seu periodo de inatividade será o 
     * Quantum, definido em traits.h
     */ 
    static void reschedule();
    
private:
    int _id;
    Context * volatile _context;
    static Thread * _running;
    
    static Thread _main; 
    static CPU::Context _main_context;
    static Thread _dispatcher;
    static Ready_Queue _ready;
    Ready_Queue::Element _link;
    volatile State _state;

    /*
     * Qualquer outro atributo que você achar necessário para a solução.
     */
    static int _countID;   //id das threads
    int _exit_code;        //codigo de erro
    Thread * _suspend;   // Thread que chamou join
    static Ordered_List<Thread> _suspended; //Fila de threads suspensas
    Ordered_List<Thread> *_wait; //pra saber qual fila de waiting de qual semáforo a thread está
    //Trabalho 6
    static Timer *_timer;
};

template<typename ... Tn>
inline Thread::Thread(void (* entry)(Tn ...), Tn ... an) : _link(this,(std::chrono::duration_cast<std::chrono::microseconds>
    (std::chrono::high_resolution_clock::now().time_since_epoch()).count())) /* inicialização de _link */
{
    //IMPLEMENTAÇÃO DO CONSTRUTOR
    db<Thread>(TRC) << "TRC: Thread(Tn ... an)\n";
    _context = new CPU::Context(entry,an...);
    _id = _countID;
    db<Thread>(TRC) << "TRC: Construiu Thread -> " << _countID << "\n";
    _countID++;
    _state = READY;
    _ready.insert(&_link);
    // Trabalho 4
    _exit_code = 0;
    _suspend = nullptr;
    // Trabalho 5
    _wait = 0;
}

__END_API

#endif
