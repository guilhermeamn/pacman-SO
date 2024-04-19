#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
#include "srv/traits.h"

__BEGIN_API

class Interface {
public:
    typedef enum {W, G, P, u, o, e, O, E, F} tile;
    //--------------matriz----------------//
    static int maze[31][28];
    static int mazeDefault[31][28]; //salvar maze inicial para quando der restart no jogo
    static int posicoes[31][28]; //matriz com posições de todos personagens (proteger com semáforo)
    //------------conversão--------------//
    static int convert_matriz_pixel_x(int posY);
    static int convert_matriz_pixel_y(int posX);
};

__END_API

#endif