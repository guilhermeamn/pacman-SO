#include "cpu.h"
#include <iostream>
#include <bits/stdc++.h> //para usar comando assembly

__BEGIN_API

void CPU::Context::save()
{
    //adicionar implementação
    if(setcontext(&_context) == -1) {
        std::cout << "ERRO setcontext" << std::endl;
        std::exit(-1);
    }
}

void CPU::Context::load()
{
    //adicionar implementação
    if(getcontext(&_context) == -1) {
        std::cout << "ERRO getcontext" << std::endl;
        std::exit(-1);
    }
}

CPU::Context::~Context()
{
    //adicionar implementação
    delete _stack;
}

void CPU::switch_context(Context *from, Context *to)
{
     //implementação do método
     if(swapcontext(&from->_context,&to->_context) == -1) {
        std::cout << "ERR: Erro na CPU::switch_context()\n";
        std::exit(-1);
     }
}

/* =========================================== */
/*  TEMP ← SRC + DEST;
    SRC ← DEST;      -> a = b
    DEST ← TEMP;     -> b = soma
*/
//number recebe o valor do registrador ebx, que é a soma
/* =========================================== */
int CPU::finc(volatile int & number)
{
    int val = 1;
    int _valor_ant_number = number;
    __asm__ __volatile__("lock xadd %0, %2": "=a"(val): "a"(val), "m"(number): "memory");
    db<CPU>(TRC) << "CPU::finc: " << number << "\n";
    return _valor_ant_number;
}

int CPU::fdec(volatile int & number)
{
    int val = -1;
    int _valor_ant_number = number;
    __asm__ __volatile__("lock xadd %0, %2": "=a"(val): "a"(val), "m"(number): "memory");
    db<CPU>(TRC) << "CPU::fdec: " << number << "\n";
    return _valor_ant_number;
}

__END_API