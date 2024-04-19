#ifndef PACMAN_H
#define PACMAN_H

#include <iostream>
#include "srv/traits.h"
#include "keyboard.h"
#include "window.h"

__BEGIN_API

class Pacman {
private:
    //----posição em pixels----//
    int positionX;
    int positionY;
    //---posição [i][j] na matriz---//
    int posX_maze;
    int posY_maze;
    float angle;
    int pill;
    int life;
    int bigPill;
    int fruit;
    int points;
    int debouncerPacman;
public:
    typedef enum {W, G, P, u, o, e, O, E, F} tile;
    //------construtor-----//
    Pacman();
    Pacman(int x, int y);
    //-------auxiliares------//
    int run();
    void atualizaPontos();
    void volta_posicao_inicial();
    int check(float angulo, int dir, int x_matriz, int y_matriz, int eixo);
    void reset();
    //---------sets--------//
    void setPositionX(int x);
    void setPositionY(int y);
    void setPosX_maze(int x);
    void setPosY_maze(int y);
    void setAngle(float a);
    void setPill(int p);
    void setLife(int l);
    void setBigPill(int b);
    void setFruit(int f);
    void setPoints(int p);
    void set_debouncerPacman(int d);
    //--------gets---------//
    int getPositionX() const;
    int getPositionY() const;
    int getPosX_maze() const;
    int getPosY_maze() const;
    float getAngle() const;
    int getPill() const;
    int getLife() const;
    int getBigPill() const;
    int getFruit() const;
    int getPoints() const;
    int get_debouncerPacman() const;
    //-----destrutor------//
    ~Pacman();
};

__END_API

#endif