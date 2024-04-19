#ifndef GHOST_H
#define GHOST_H

#include <iostream>
#include "srv/traits.h"

__BEGIN_API

/*
!!!!!!! AQUI ESTÁ EM PIXEL !!!!!!
Na caixa   [ PINK, BLUE AND RED, YELLOW ]

BLUE   [ 233, 234 ]   RED
PINK   [ 233, 203 ]
YELLOW [ 233, 265 ]
*/

#define BLUE_Xo 234
#define BLUE_Yo 233
#define PINK_Xo 203
#define PINK_Yo 233
#define YELLOW_Xo 265
#define YELLOW_Yo 233

#define RED_CADEIA_Xo 234
#define RED_CADEIA_Yo 233

#define RED_Xo  233
#define RED_Yo  172

class Ghost {
public:
    typedef enum {ATACANTE = 1, FUGITIVO, MORTO, SLEEP, INIT, WAITING} estado_ghost;
    typedef enum {RED = 1, BLUE, YELLOW, PINK} type_ghosts;
    /*
    Recebe x e y para pixel inicial
    */
    Ghost(int x, int y, type_ghosts tipo);
    ~Ghost();

    void reset();

    int atualiza_movimento_fantasma(int _x_maze_pacman, int _y_maze_pacman);
    int retorno_gradual_ao_respanw(void);
    void volta_posicao_inicial();
    void movimenta_cadeia(); //adicionado

    // ----- ESTADO DO FANTASMA ----- //
    int set_estado_fantasma(const estado_ghost entr);
    estado_ghost get_estado_fantasma(void);
    // ----- DIRECAO A IR ---------- //
    int getDir() const;
    // ----- POSICAO EM PIXEL -------- //
    int get_x_pixel(void);
    int get_y_pixel(void);
    // ----- POSICAO EM MAZE ------- //
    int get_x_maze(void);
    int get_y_maze(void);
    // ----- Movimentação pós nascimento -----//
    int movimentacao_inicial(void);

    type_ghosts get_type_ghosts(void); //adicionado
    int dentro_cadeia(int i, int j); //adicionado
    void set_debouncerGhost(int d); //adicionado
    int get_debouncerGhost() const; //adicionado
    
private:
    /*
    Posição em pixels
    */
    int _x_pixels,_y_pixels;
    /*
    Posicao em maze
    */
    int _x_maze,_y_maze;
    /*
    Infomações dos pixels iniciais
    */
    int xo_pixel,yo_pixel;
    int xo_maze, yo_maze;
    int _dir;
    int last_mov;
    estado_ghost _estado;
    type_ghosts _tipo;
    /*
        se lock_inicial estiver em 1 -> ativdado ( travado )
        o contrário livre          0 -> livre
    */
    int lock_inicial;
    int debouncerGhost;

    int posicao_valida(int i, int j);
    int hipotenusa_estrategica(int CO, int CA);
    int logica_ghost_red(int _x_maze_pacman, int _y_maze_pacman);
    int mecanica_movimento(int novaDirecao);
    int movimenta_olhos(void);

    /*
        Funções auxiliares para comportamento inicial
    */
    int logica_sair_da_cadeia(void);
    int posicao_valida_cadeia(int i, int j);
};

__END_API

#endif
