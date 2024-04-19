#include "semaphore.h"

__BEGIN_API

Semaphore::Semaphore(int v)
{
    db<Semaphore>(TRC) << "TRC: Semaphore()\n";
    _mutex = v;
}

void Semaphore::p()
{
    db<Semaphore>(TRC) << "TRC: p()\n";
    if(fdec(_mutex) <= 0){
        sleep();
    }
}

void Semaphore::v()
{
    db<Semaphore>(TRC) << "TRC: v()\n";
    _mutex = finc(_mutex);
    /* Sempre precisa acordar um thread
        mas precisa ser possível
    */
    if (_waiting.empty())
        return;
    wakeup();
}

int Semaphore::finc(volatile int & number)
{
    db<Semaphore>(TRC) << "TRC: finc()\n";
    return CPU::finc(number);
}

int Semaphore::fdec(volatile int & number)
{
    db<Semaphore>(TRC) << "TRC: fdec()\n";
    return CPU::fdec(number);;
}

void Semaphore::sleep()
{
    db<Semaphore>(TRC) << "TRC: sleep()\n";
    Thread::sleep(&_waiting);
    Thread::yield();
}

void Semaphore::wakeup()
{
    db<Semaphore>(TRC) << "TRC: wakeup()\n";
    Thread::wakeup(&_waiting);
    Thread::yield();
}

void Semaphore::wakeup_all()
{
    db<Semaphore>(TRC) << "TRC: wakeup_all()\n";
    while (_waiting.size()){
        Thread::wakeup(&_waiting);
    }
    Thread::yield();
}

Semaphore::~Semaphore()
{
    db<Semaphore>(TRC) << "TRC: ~Semaphore()\n";
    wakeup_all();
}

__END_API