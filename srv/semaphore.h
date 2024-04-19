#ifndef semaphore_h
#define semaphore_h

#include "cpu.h"
#include "thread.h"
#include "traits.h"
#include "debug.h"
#include "list.h"
#include <ctime> 
#include <chrono>

__BEGIN_API

class Semaphore
{
public:
    Semaphore(int v = 1);
    ~Semaphore();

    void p();
    void v();
    /*
    BLINDAGEM - processo finalizado e que estava dormindo,
    precisa ser finalizado
    */

private:
    // Atomic operations
    int finc(volatile int & number);
    int fdec(volatile int & number);

    // Thread operations
    void sleep(); // add parametro para passar o ponteiro da fila de prontos, para ambos
    void wakeup();
    void wakeup_all();

private:
    //DECLARAÇÃO DOS ATRIBUTOS DO SEMÁFORO
    int _mutex; //atributo de controle da região crítica
    /* Fila de thread que estão dormindo por este semaforo
    --> as manipulações obrigatoriamente tem que ser feitas na thread
    */
    Ordered_List<Thread> _waiting;
};

__END_API

#endif
