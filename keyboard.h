#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <iostream>
#include "srv/traits.h"
#include "window.h"

__BEGIN_API

class Keyboard 
{
public:
    Keyboard();
    ~Keyboard();
    static std::string get_tecla_pressionada(void);
    //---------------botões---------------//
    static bool left;
    static bool right;
    static bool up;
    static bool down;
    static bool paused;
    static bool reset;
    //----------------run-----------------//
    static void run();
    /*
        Buffer de evento
        -> Não pode ser obtido diretamente
    */
    static std::string buffer_teclado;
};

__END_API

#endif