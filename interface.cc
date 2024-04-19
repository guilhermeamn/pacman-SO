#include "interface.h"

__BEGIN_API

/*
 * Converte posição na matriz em pixel para x (tira uma média do intervalo de pixels)
 * OBS.: se matriz[i][j] -> convert_matriz_x(j)
 *                          convert_matriz_y(i)
 */
int Interface::convert_matriz_pixel_x(int posY)
{
    //db<Interface>(TRC) << "TRC: convert_matriz_pixel_x(int)\n";

    double Mx = 435/28.0; //15.5357 (pixels/tamanho)

    double x_min;
    double x_max;

    //--------intervalo em pixels (posY*Mx <= valor <= (posY+1)*Mx)-------//
    x_min = posY*Mx;
    x_max = (posY+1)*Mx;
    
    int media = (x_min + x_max) / 2; //pega o valor médio do intervalo

    return media;
}

/*
 * Converte posição na matriz em pixel para y (tira uma média do intervalo de pixels)
 * OBS.: se matriz[i][j] -> convert_matriz_x(j)
 *                          convert_matriz_y(i)
 */
int Interface::convert_matriz_pixel_y(int posX)
{
    //db<Interface>(TRC) << "TRC: convert_matriz_pixel_y(int)\n";

    double My = 485/31.0; //15.6452 (pixels/tamanho)

    double y_min;
    double y_max;

    //--------intervalo em pixels (posX*My <= valor <= (posX+1)*My)-------//
    y_min = posX*My;
    y_max = (posX+1)*My;

    int media = (y_min + y_max) / 2; //pega o valor médio do intervalo

    return media;
}

/**
 * Tile defined as an enum where each type is defined as:
 *      W: Wall
 *      G: Ghost Pen Gate
 *      P: Portal
 *      u: Empty path
 *      o: Pill
 *      e: Eaten Pill
 *      O: Big Pill
 *      E: Eaten Big Pill
 *      F: Fruit
 */
int Interface::maze[31][28] = 
    {
                {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W},
                {W,o,o,o,o,o,o,o,o,o,o,o,o,W,W,o,o,o,o,o,o,o,o,o,o,o,o,W},
                {W,o,W,W,W,W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,o,W,W,W,W,o,W},
                {W,o,W,W,W,W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,o,W,W,W,W,o,W},
                {W,O,W,W,W,W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,o,W,W,W,W,O,W},
                {W,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,W},
                {W,o,W,W,W,W,o,W,W,o,W,W,W,W,W,W,W,W,o,W,W,o,W,W,W,W,o,W},
                {W,o,W,W,W,W,o,W,W,o,W,W,W,W,W,W,W,W,o,W,W,o,W,W,W,W,o,W},
                {W,o,o,o,o,o,o,W,W,o,o,o,o,W,W,o,o,o,o,W,W,o,o,o,o,o,o,W},
                {W,W,W,W,W,W,o,W,W,W,W,W,u,W,W,u,W,W,W,W,W,o,W,W,W,W,W,W},
                {u,u,u,u,u,W,o,W,W,W,W,W,u,W,W,u,W,W,W,W,W,o,W,u,u,u,u,u},
                {u,u,u,u,u,W,o,W,W,u,u,u,u,u,u,u,u,u,u,W,W,o,W,u,u,u,u,u},
                {u,u,u,u,u,W,o,W,W,u,W,W,W,G,G,W,W,W,u,W,W,o,W,u,u,u,u,u},
                {W,W,W,W,W,W,o,W,W,u,W,u,u,u,u,u,u,W,u,W,W,o,W,W,W,W,W,W},
                {P,u,u,u,u,u,o,u,u,u,W,u,u,u,u,u,u,W,u,u,u,o,u,u,u,u,u,P},
                {W,W,W,W,W,W,o,W,W,u,W,u,u,u,u,u,u,W,u,W,W,o,W,W,W,W,W,W},
                {u,u,u,u,u,W,o,W,W,u,W,W,W,W,W,W,W,W,u,W,W,o,W,u,u,u,u,u},
                {u,u,u,u,u,W,o,W,W,u,u,u,u,u,u,u,u,u,u,W,W,o,W,u,u,u,u,u},
                {u,u,u,u,u,W,o,W,W,u,W,W,W,W,W,W,W,W,u,W,W,o,W,u,u,u,u,u},
                {W,W,W,W,W,W,o,W,W,u,W,W,W,W,W,W,W,W,u,W,W,o,W,W,W,W,W,W},
                {W,o,o,o,o,o,o,o,o,o,o,o,o,W,W,o,o,o,o,o,o,o,o,o,o,o,o,W},
                {W,o,W,W,W,W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,o,W,W,W,W,o,W},
                {W,o,W,W,W,W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,o,W,W,W,W,o,W},
                {W,O,o,o,W,W,o,o,o,o,o,o,o,u,u,o,o,o,o,o,o,o,W,W,o,o,O,W},
                {W,W,W,o,W,W,o,W,W,o,W,W,W,W,W,W,W,W,o,W,W,o,W,W,o,W,W,W},
                {W,W,W,o,W,W,o,W,W,o,W,W,W,W,W,W,W,W,o,W,W,o,W,W,o,W,W,W},
                {W,o,o,o,o,o,o,W,W,o,o,o,o,W,W,o,o,o,o,W,W,o,o,o,o,o,o,W},
                {W,o,W,W,W,W,W,W,W,W,W,W,o,W,W,o,W,W,W,W,W,W,W,W,W,W,o,W},
                {W,o,W,W,W,W,W,W,W,W,W,W,o,W,W,o,W,W,W,W,W,W,W,W,W,W,o,W},
                {W,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,W},
                {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}
    };

int Interface::mazeDefault[31][28] = { Interface::maze[31][28] };

int Interface::posicoes[31][28];

__END_API