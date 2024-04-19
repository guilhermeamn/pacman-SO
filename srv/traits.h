#ifndef traits_h
#define traits_h

//Não alterar as 3 declarações abaixo

#define __BEGIN_API             namespace SOLUTION {
#define __END_API               }
#define __USING_API            using namespace SOLUTION;

__BEGIN_API

//SEMPRE COLOCAR BEGIN API E END API NOS .H E .CC

//declaração das classes criadas nos trabalhos devem ser colocadas aqui
class CPU;
class Debug;
class System;
class Thread;
class Lists;
class Semaphore;
class Timer;
// Jogo
class Ghost;
class Pacman;
class Keyboard;
class Window;

//declaração da classe Traits
template<typename T>
struct Traits {
private:
    /*void * ss_sp;                   // ponteiro para a memoria da pilha
    int ss_flags;                   // Sinalizador, vou usar default = 0
    size_t ss_size;                 // Tamanho da pilha*/
    static const bool debugged = false;
};

template<> struct Traits<CPU>
{
    //Adicionar aqui a declaração do tamanho da Pilha -> STACK_SIZE
    static const unsigned int STACK_SIZE = 64000;
    static const bool debugged = false;
};

template<> struct Traits<Debug>: public Traits<void>
{
    static const bool error = true;
    static const bool warning = true;
    static const bool info = true;
    static const bool trace = true;
};

template<> struct Traits<System> : public Traits<void>
{
    static const bool debugged = false;
};

template<> struct Traits<Thread> : public Traits<void>
{
    static const bool debugged = false;
};

template<> struct Traits<Lists> : public Traits<void>
{
    static const bool debugged = false;
};

template<> struct Traits<Semaphore> : public Traits<void>
{
    static const bool debugged = false;
};

template<> struct Traits<Timer> : public Traits<void>
{
    static const bool debugged = false;
    static const bool active_timer = false; //atributo pra ativar ou não timer
    static const unsigned int quantum = 45000; // 70000 (45000 tá bom)

};

// Jogo
template<> struct Traits<Window> : public Traits<void>
{
    static const bool debugged = false;
};

template<> struct Traits<Keyboard> : public Traits<void>
{
    static const bool debugged = false;
};

template<> struct Traits<Pacman> : public Traits<void>
{
    static const bool debugged = false;
};


__END_API

#endif
