#ifndef window_h
#define window_h

#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include "srv/traits.h"
#include "srv/thread.h"
#include "ghost.h"
#include "interface.h"
#include "pacman.h"

__BEGIN_API

class Window
{
public:
    static sf::RenderWindow *janela;
    static sf::Event *eventos;
public:
    typedef enum {W, G, P, u, o, e, O, E, F} tile;
    typedef enum {INICIO,RUNNING,PAUSE,MORREU,GAMEOVER,WIN,RESTART} MODE;

    Window();
    //---------------------thread---------------------//
    void run();
    void runGhost(int i);
    void runPacman();
    //---------------pacman--------------//
    static Pacman *pacman;
    //-------------fantasmas-------------//
    static Ghost *ghosts[4];
    //----------------modo de jogo--------------------//
    static MODE modo;
    //---------------métodos auxiliares---------------//
    static int convert(int pixelX, int pixelY, int &posX, int&posY);
    void initPoints();
    void initWalkPacman();
    void initDeadPacman();
    void initGhostEyes();
    void initGhostsSprites();
    //-------------teclado--------------//
    static std::string key;
    //-------------fruta---------------//
    static bool fruitEat;
    //-----------comeu fantasma--------//
    static bool ghostEat[4];
    // --------- ANIMACOES ----------- //
    // --> PACMAN
    void anima_pacman_boca(sf::RenderWindow &janela);
    void anima_pacman_morte(sf::RenderWindow &janela);
    void anima_comeu_fantasma(sf::RenderWindow &janela, int g);
    // --> FANTASMA
    void anima_fantasma(sf::RenderWindow &janela, int g);
    // --> Objetos
    void anima_pill_and_bigpill(sf::RenderWindow &janela);
    void anima_pontuacao(sf::RenderWindow &janela);
    void anima_vidas(sf::RenderWindow &janela);
    void anima_fruta(sf::RenderWindow &janela);

    void draw_texture(unsigned int texture, int length, int height, float angle);
private:
    void load_and_bind_textures();

private:
    //---------------delay---------------//
    int debouncer = 0;
    int debouncerFruit = 0;
    int debouncerGhost[4] = {0,0,0,0};
    int debouncerGhost_aux[4] = {0,0,0,0};
    int debouncerEatGhost[4] = {0,0,0,0};
    int debouncerGhostSleep[4] = {0,0,0,0};
    int tempoInit[4] = {0,0,0,0};
    int tempo[4] = {0,0,0,0}; //será o valor aleatório que o fantasma vai ficar na cadeia
    int debouncerMorreu = 0;
    int debouncerWalkPacman = 0;
    //----------------menu---------------//
    int points = 0;
    sf::Sprite numeros[10];
    sf::Sprite walk_pacman[3];
    sf::Sprite dead_pacman[11];
    sf::Sprite ghost_eyes[4];
    sf::Sprite ghosts_sprites[4];
    int dead = 0;
    int walk = 0;
    bool aumenta = true;
    //matriz das posições em que o fantasma morreu: pos_ghost_morreu[id][x ou y]
    int pos_ghost_morreu[4][2] = {{0,0},
                                  {0,0},
                                  {0,0},
                                  {0,0}};

    // Maze Textures
    sf::Texture maze_tex;
    sf::Sprite maze_sprite;
    sf::Texture pill_tex;
    sf::Sprite pill_sprite;
    sf::Texture bigPill_tex;
    sf::Sprite bigPill_sprite;

    // Pacman Textures
    sf::Texture pac_0_tex;
    sf::Sprite pac_0_sprite;
    sf::Texture pac_1_tex;
    sf::Sprite pac_1_sprite;
    sf::Texture pac_2_tex;
    sf::Sprite pac_2_sprite;
    sf::Texture dead_0_tex;
    sf::Sprite dead_0_sprite;
    sf::Texture dead_1_tex;
    sf::Sprite dead_1_sprite;
    sf::Texture dead_2_tex;
    sf::Sprite dead_2_sprite;
    sf::Texture dead_3_tex;
    sf::Sprite dead_3_sprite;
    sf::Texture dead_4_tex;
    sf::Sprite dead_4_sprite;
    sf::Texture dead_5_tex;
    sf::Sprite dead_5_sprite;
    sf::Texture dead_6_tex;
    sf::Sprite dead_6_sprite;
    sf::Texture dead_7_tex;
    sf::Sprite dead_7_sprite;
    sf::Texture dead_8_tex;
    sf::Sprite dead_8_sprite;
    sf::Texture dead_9_tex;
    sf::Sprite dead_9_sprite;
    sf::Texture dead_10_tex;
    sf::Sprite dead_10_sprite;

    // Ghost Textures
    sf::Texture ghost_r_0_tex;
    sf::Sprite ghost_r_0_sprite;
    sf::Texture ghost_r_1_tex;
    sf::Sprite ghost_r_1_sprite;
    sf::Texture ghost_p_0_tex;
    sf::Sprite ghost_p_0_sprite;
    sf::Texture ghost_p_1_tex;
    sf::Sprite ghost_p_1_sprite;
    sf::Texture ghost_b_0_tex;
    sf::Sprite ghost_b_0_sprite;
    sf::Texture ghost_b_1_tex;
    sf::Sprite ghost_b_1_sprite;
    sf::Texture ghost_y_0_tex;
    sf::Sprite ghost_y_0_sprite;
    sf::Texture ghost_y_1_tex;
    sf::Sprite ghost_y_1_sprite;
    sf::Texture ghost_scared_0_tex;
    sf::Sprite ghost_scared_0_sprite;
    sf::Texture ghost_scared_1_tex;
    sf::Sprite ghost_scared_1_sprite;
    sf::Texture ghost_scared_2_tex;
    sf::Sprite ghost_scared_2_sprite;
    sf::Texture ghost_scared_3_tex;
    sf::Sprite ghost_scared_3_sprite;

    // Eye Textures
    sf::Texture eye_up_tex;
    sf::Sprite eye_up_sprite;
    sf::Texture eye_right_tex;
    sf::Sprite eye_right_sprite;
    sf::Texture eye_down_tex;
    sf::Sprite eye_down_sprite;
    sf::Texture eye_left_tex;
    sf::Sprite eye_left_sprite;

    // UI Textures
    sf::Texture num_0_tex;
    sf::Sprite num_0_sprite;
    sf::Texture num_1_tex;
    sf::Sprite num_1_sprite;
    sf::Texture num_2_tex;
    sf::Sprite num_2_sprite;
    sf::Texture num_3_tex;
    sf::Sprite num_3_sprite;
    sf::Texture num_4_tex;
    sf::Sprite num_4_sprite;
    sf::Texture num_5_tex;
    sf::Sprite num_5_sprite;
    sf::Texture num_6_tex;
    sf::Sprite num_6_sprite;
    sf::Texture num_7_tex;
    sf::Sprite num_7_sprite;
    sf::Texture num_8_tex;
    sf::Sprite num_8_sprite;
    sf::Texture num_9_tex;
    sf::Sprite num_9_sprite;
    sf::Texture score_200_tex;
    sf::Sprite score_200_sprite;
    sf::Texture score_400_tex;
    sf::Sprite score_400_sprite;
    sf::Texture score_800_tex;
    sf::Sprite score_800_sprite;
    sf::Texture score_1600_tex;
    sf::Sprite score_1600_sprite;
    sf::Texture ready_tex;
    sf::Sprite ready_sprite;
    sf::Texture gameover_tex;
    sf::Sprite gameover_sprite;
    sf::Texture life_tex;
    sf::Sprite life_sprite;
    sf::Texture score_tex;
    sf::Sprite score_sprite;
    sf::Texture high_tex;
    sf::Sprite high_sprite;
    sf::Texture score_100_tex;
    sf::Sprite score_100_sprite;
    sf::Texture score_300_tex;
    sf::Sprite score_300_sprite;

    // Fruit Textures
    sf::Texture cherry_tex;
    sf::Sprite cherry_sprite;
    sf::Texture strawberry_tex;
    sf::Sprite strawberry_sprite;
};

__END_API

#endif