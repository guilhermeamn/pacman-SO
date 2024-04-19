#ifndef cpu_h
#define cpu_h

#include <ucontext.h>
#include <iostream>
#include "traits.h"
#include "debug.h"

__BEGIN_API

class CPU
{
    public:

        class Context
        {
        private:
            static const unsigned int STACK_SIZE = Traits<CPU>::STACK_SIZE;
        public:
            Context() { 
                _stack = 0;
                load(); //get context, inicializa o contexto
            }

            template<typename ... Tn>
            Context(void (* func)(Tn ...), Tn ... an) {
                _stack = new char[STACK_SIZE]; //aloca memoria pra pilha de execucao
                load(); //get context, inicializa o contexto
                _context.uc_link = 0; //seta valores
                _context.uc_stack.ss_sp = _stack;
                _context.uc_stack.ss_size = STACK_SIZE;
                _context.uc_stack.ss_flags = 0;
                makecontext(&_context,(void(*)(void))func,sizeof...(Tn),an...); //associa a função ao contexto
            }

            ~Context();

            void save();
            void load();

        private:            
            char *_stack; //pilha de exec
        public:
            ucontext_t _context;
        };

    public:
        static void switch_context(Context *from, Context *to);
        /* Implementações do Trabalho 5
        -->  Atomic operations 
        --> Retorna o valor anterior de number
        */
        static int finc(volatile int & number);
        static int fdec(volatile int & number);

};

__END_API

#endif
