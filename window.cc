#include "window.h"
#include "main_class.h"

__BEGIN_API

//-----------------------Atributos static-----------------------//
Pacman *Window::pacman = 0;
Ghost *Window::ghosts[4];
std::string Window::key = "";
bool Window::fruitEat = false;
Window::MODE Window::modo;
bool Window::ghostEat[4] {false,false,false,false};
sf::RenderWindow *Window::janela = 0;
sf::Event *Window::eventos;

Window::Window()
{
    load_and_bind_textures();
}

void Window::draw_texture(unsigned int texture, int length, int height, float angle)
{
}

/*
    Função que sempre é iterada externamente
    Responsavel por atualizar a posição do pacman
*/
void Window::runPacman()
{
    db<Window>(TRC) << "TRC: runPacman\n";

    if(!Main::end)
    {
        db<Window>(TRC) << "TRC: PACMAN TÁ AQUI\n";

        pacman->set_debouncerPacman(pacman->get_debouncerPacman() + 1);
        if( (!Traits<Timer>::active_timer && pacman->get_debouncerPacman() == 3)
            || (Traits<Timer>::active_timer) )
        {
            pacman->set_debouncerPacman(0);

            //-----------------------Run Pacman--------------------//
            int m = pacman->run();

            if(m)
            {
                for(int g = 0; g < 4; g++) ghosts[g]->set_estado_fantasma(Ghost::FUGITIVO);
            }
        }
        
        //-------------delay-----------//
        if(Traits<Timer>::active_timer) for (int i = 0; i < 40000000; i++){}
    }

    else
    {
        Main::endPacman = true;
        db<Window>(TRC) << "TRC: endPacman\n";
    }

    db<Window>(TRC) << "TRC: runPacmanFinal\n";
}

/*
    Função que sempre é iterada externamente
    Responsavel por atualizar a posição do fantasma
    @param: i -> identificação da thread
*/

void Window::runGhost(int i)
{
    db<Window>(TRC) << "TRC: runGhost: " << i << "\n";
    if(!Main::end)
    {
        ghosts[i]->set_debouncerGhost(ghosts[i]->get_debouncerGhost() + 1);
        if( (!Traits<Timer>::active_timer && ghosts[i]->get_debouncerGhost() == 6) 
            || (Traits<Timer>::active_timer) )
        {
            ghosts[i]->set_debouncerGhost(0);

            if(modo != INICIO && modo != PAUSE && modo != RESTART)
            {
                //-------------------Pacman e fantasma na mesma posição------------------//
                if(pacman->getPosX_maze() == ghosts[i]->get_x_maze() && pacman->getPosY_maze() == ghosts[i]->get_y_maze())
                {
                    db<Window>(INF) << "INF: Encostou: " << i << "\n";
                    //---------------fantasma ATACANTE--------------//
                    if(ghosts[i]->get_estado_fantasma() == Ghost::ATACANTE)
                    {
                        db<Window>(INF) << "INF: Pacman morreu: " << i << "\n";

                        pacman->setLife(pacman->getLife() - 1);
                        if(pacman->getLife() <= 0) modo = GAMEOVER;
                        else modo = MORREU;
                    }
                    //---------------fantasma FUGITIVO--------------//
                    else if(ghosts[i]->get_estado_fantasma() == Ghost::FUGITIVO)
                    {
                        db<Window>(INF) << "INF: Fantasma morreu: " << i << "\n";

                        ghostEat[i] = true;
                        pacman->setPoints(pacman->getPoints() + 200);
                        ghosts[i]->set_estado_fantasma(Ghost::MORTO);
                    }
                }

                /*

                */
                if(modo == MORREU || modo == GAMEOVER)
                {
                    db<Window>(INF) << "INF: Pacman morreu, volta posições iniciais: " << i << "\n";
                    ghosts[i]->volta_posicao_inicial();
                    //fantasmas (menos o vermelho) voltam a esperar seu tempo pra sair da cadeia
                    if(ghosts[i]->get_type_ghosts() != Ghost::RED) ghosts[i]->set_estado_fantasma(Ghost::INIT);
                }
                /*
                    Trata os movimentos de ataque e fuga do fantasma
                */
                if ( ghosts[i]->get_estado_fantasma() == Ghost::ATACANTE || ghosts[i]->get_estado_fantasma() == Ghost::FUGITIVO)
                {
                    db<Window>(INF) << "INF: Ghost::ATACANTE ou Ghost::FUGITIVO: " << i << "\n";
                    ghosts[i]->atualiza_movimento_fantasma(pacman->getPosX_maze(),pacman->getPosY_maze());
                }
                /*
                    Trata movimento de morto ( anima olhos )
                */
                if ( ghosts[i]->get_estado_fantasma() == Ghost::MORTO){
                    db<Window>(INF) << "INF: Ghost::MORTO: " << i << "\n";
                    ghosts[i]->retorno_gradual_ao_respanw();

                    //para o vermelho, posição inicial não é dentro da cadeia, então checa manualmente
                    if( ghosts[i]->get_type_ghosts() == Ghost::RED)
                    {
                        //se vermelho dentro da cadeia, seta pra WAITING
                        if( ghosts[i]->dentro_cadeia(ghosts[i]->get_x_maze(),ghosts[i]->get_y_maze()))
                            ghosts[i]->set_estado_fantasma(Ghost::WAITING);
                    }
                }
                /*
                    Trata movimentos iniciais
                */
                if( ghosts[i]->get_estado_fantasma() == Ghost::SLEEP){
                    db<Window>(INF) << "INF: Ghost::SLEEP: " << i << "\n";
                    ghosts[i]->movimentacao_inicial(); // trata movimento incial
                }

                if( ghosts[i]->get_estado_fantasma() == Ghost::WAITING){
                    db<Window>(INF) << "INF: Ghost::WAITING: " << i << "\n";

                    ghosts[i]->movimenta_cadeia();

                    //tempo ainda não foi inicializado
                    if(tempo[i] == 0)
                    {
                        //gera número aleatório entre 1 e 7
                        unsigned seed = time(0);
                        srand(seed);
                        tempo[i] = 1 + rand() % 7;

                        db<Window>(INF) << "<><><><><>< tempo = " << tempo[i] << "\n";
                    }

                    debouncerGhostSleep[i]++;
                    if(debouncerGhostSleep[i] == tempo[i])
                    {
                        ghosts[i]->set_estado_fantasma(Ghost::SLEEP);
                        tempo[i] = 0;
                        debouncerGhostSleep[i] = 0;
                    }
                }

                /*
                    Trata movimentos iniciais dos outros fantasmas
                    Movimenta o fantasma apenas dentro da cadeia até que seja liberado
                    Cada fantasma é liberado após um determinado tempo
                */
                if( ghosts[i]->get_estado_fantasma() == Ghost::INIT){
                    db<Window>(INF) << "INF: Ghost::INIT: " << i << "\n";

                    ghosts[i]->movimenta_cadeia();
                    tempoInit[i]++;

                    if( (!Traits<Timer>::active_timer && ghosts[i]->get_type_ghosts() == Ghost::BLUE && tempoInit[i] >= 30) 
                        || (Traits<Timer>::active_timer && ghosts[i]->get_type_ghosts() == Ghost::BLUE && tempoInit[i] >= 10) )
                    {
                        db<Window>(INF) << "INF: Ghost::BLUE: " << i << "\n";
                        ghosts[i]->set_estado_fantasma(Ghost::SLEEP);
                        tempoInit[i] = 0;
                    }
                    else if( (!Traits<Timer>::active_timer && ghosts[i]->get_type_ghosts() == Ghost::YELLOW && tempoInit[i] >= 70) 
                        || (Traits<Timer>::active_timer && ghosts[i]->get_type_ghosts() == Ghost::YELLOW && tempoInit[i] >= 25) )
                    {
                        db<Window>(INF) << "INF: Ghost::YELLOW: " << i << "\n";
                        ghosts[i]->set_estado_fantasma(Ghost::SLEEP);
                        tempoInit[i] = 0;
                    }
                    else if( (!Traits<Timer>::active_timer && ghosts[i]->get_type_ghosts() == Ghost::PINK && tempoInit[i] >= 120) 
                        || (Traits<Timer>::active_timer && ghosts[i]->get_type_ghosts() == Ghost::PINK && tempoInit[i] >= 40) )
                    {
                        db<Window>(INF) << "INF: Ghost::PINK: " << i << "\n";
                        ghosts[i]->set_estado_fantasma(Ghost::SLEEP);
                        tempoInit[i] = 0;
                    }
                }
            }    
        }

        //------------delay----------------//
        if(Traits<Timer>::active_timer) for (int i = 0; i < 60000000; i++){}
    }

    else Main::endGhosts[i] = true;
    
    db<Window>(TRC) << "TRC: runGhostFinal: " << i << "\n";
}

/*
 * Tela tem +/- 435x485 pixels
 * Matriz tem 28x31 posições (é invertido, [i][j] -> i equivale ao y, j equivale ao x)
 * Cada posição da matriz possui em x: 435/28 = 15.5357 pixels
 *                               em y: 485/31 = 15.6452 pixels
 * Pra checar se o pacman está em tal posição da matriz, checa intervalo de pixels
 * Por exemplo: Na posição maze[0][1] -> em x: 15.5357 < pacmanX < 2*15.5357
 *                                       em y: 0 < pacmanY < 15.6452
 * Genéricamente: maze[i][j] -> em x: j*15.5357 < pacmanX < (j+1)*15.5357
 *                              em y: i*15.6452 < pacmanY < (i+1)*15.6452
 * Retorna o valor da matriz na posição [i][j]
 * Armazena em posX e posY, a posição [i][j] na matriz
 */
int Window::convert(int pixelX, int pixelY, int &posX, int &posY)
{
    db<Window>(TRC) << "TRC: convert(int,int,int&,int&)\n";

    //--------qtade de pixels de cada posição na matriz (pixels/tamanho) -------//
    double Mx = 435/28.0; //15.5357
    double My = 485/31.0; //15.6452

    db<Window>(INF) << "INF: Mx = " << Mx << "\n";
    db<Window>(INF) << "INF: My = " << My << "\n";

    for(int i = 0; i < 31; i++)
    {
        for(int j = 0; j < 28; j++)
        {
            if( (j*Mx <= pixelX) && (pixelX <= (j+1)*Mx) )
            {
                if( (i*My <= pixelY) && (pixelY <= (i+1)*My) )
                {
                    db<Window>(INF) << "INF: pixel aqui -> " << "[" << i << "][" << j << "]\n";
                    db<Window>(INF) << Interface::maze[i][j] << "\n";
                    posX = i;
                    posY = j;
                    return Interface::maze[i][j];
                }
            }
        }
    }

    db<Window>(ERR) << "ERR: Pixel não encontrado na matriz\n";
    return -1;
}

//-----------Inicializa o vetor de pontuação (não consegui atribuir direto no .h)------//
void Window::initPoints()
{
    db<Window>(TRC) << "TRC: initPoints()\n";

    numeros[0] = num_0_sprite;
    numeros[1] = num_1_sprite;
    numeros[2] = num_2_sprite;
    numeros[3] = num_3_sprite;
    numeros[4] = num_4_sprite;
    numeros[5] = num_5_sprite;
    numeros[6] = num_6_sprite;
    numeros[7] = num_7_sprite;
    numeros[8] = num_8_sprite;
    numeros[9] = num_9_sprite;

}

//----------------Inicializa o vetor dos pacman andando---------------//
void Window::initWalkPacman()
{
    db<Window>(TRC) << "TRC: initWalkPacman()\n";

    walk_pacman[0] = pac_2_sprite;
    walk_pacman[1] = pac_1_sprite;
    walk_pacman[2] = pac_0_sprite;
}

//----------------Inicializa o vetor dos dead_pacman-----------------//
void Window::initDeadPacman()
{
    db<Window>(TRC) << "TRC: initDeadPacman()\n";

    dead_pacman[0] = dead_0_sprite;
    dead_pacman[1] = dead_1_sprite;
    dead_pacman[2] = dead_2_sprite;
    dead_pacman[3] = dead_3_sprite;
    dead_pacman[4] = dead_4_sprite;
    dead_pacman[5] = dead_5_sprite;
    dead_pacman[6] = dead_6_sprite;
    dead_pacman[7] = dead_7_sprite;
    dead_pacman[8] = dead_8_sprite;
    dead_pacman[9] = dead_9_sprite;
    dead_pacman[10] = dead_10_sprite;
}

//-------------Inicializa o vetor dos olhos do fantasma-------------//
void Window::initGhostEyes()
{
    db<Window>(TRC) << "TRC: initGhostEyes()\n";

    ghost_eyes[0] = eye_up_sprite;
    ghost_eyes[1] = eye_down_sprite;
    ghost_eyes[2] = eye_left_sprite;
    ghost_eyes[3] = eye_right_sprite;
}

//-------------Inicializa o vetor dos sprites dos fantasmas-------------//
void Window::initGhostsSprites()
{
    ghosts_sprites[0] = ghost_r_0_sprite; //vermelho
    ghosts_sprites[1] = ghost_b_0_sprite; //azul
    ghosts_sprites[2] = ghost_y_0_sprite; //amarelo
    ghosts_sprites[3] = ghost_p_0_sprite; //rosa
}

void Window::run()
{
    db<Window>(TRC) << "Window::run()\n";

    sf::RenderWindow window(sf::VideoMode(580, 500), "Pacman");

    /*
        Garantir acesso a evento assicronos da tela
    */
    janela = &window;
    /*
        Preciso garantir que os eventos existam,
        mesmo que não
    */ 
    sf::Event event;
    eventos = &event;

    //Link: https://www.sfml-dev.org/tutorials/2.5/window-events.php
    //https://www.sfml-dev.org/documentation/2.5.1/classsf_1_1Keyboard.php
    window.setKeyRepeatEnabled(false);

    pacman = new Pacman();
    /* 
        Setagem manual de cada fantasma
    */
    ghosts[0] = new Ghost(RED_Xo,RED_Yo,Ghost::RED); //RED
    ghosts[1] = new Ghost(BLUE_Xo,BLUE_Yo,Ghost::BLUE); //BLUE
    ghosts[2] = new Ghost(YELLOW_Xo,YELLOW_Yo,Ghost::YELLOW); //YELLOW
    ghosts[3] = new Ghost(PINK_Xo,PINK_Yo,Ghost::PINK); //PINK

    initPoints();
    initWalkPacman();
    initDeadPacman();
    initGhostEyes();
    initGhostsSprites();

    modo = INICIO;

    while (window.isOpen())
    {
        if ( key == "pause"){
            modo = PAUSE;
        }

        if ( Keyboard::buffer_teclado == "close"){
            Window::janela->close();
            //break;
        }
        //-------------------------Interface--------------------------//
        window.clear();

        window.draw(maze_sprite); //maze

        score_sprite.setPosition(480,20); //score
        window.draw(score_sprite);
    
        //----------------------Vidas----------------------//
        anima_vidas(window);

        //----------------------Fruta ----------------------//
        anima_fruta(window);
        //-------Se comeu a fruta, animação da pontuação por alguns segundos------//
        if(fruitEat)
        {
            debouncerFruit++;
            score_100_sprite.setPosition(Interface::convert_matriz_pixel_x(12),Interface::convert_matriz_pixel_y(17));
            window.draw(score_100_sprite);
            if( (!Traits<Timer>::active_timer && debouncerFruit == 15) 
                || (Traits<Timer>::active_timer && debouncerFruit == 1000) )
            {
                debouncerFruit = 0;
                fruitEat = false;
            }
        }
        //-------Se comeu o fantasma, animação da pontuação por alguns segundos------//
        for(int g = 0; g < 4; g++) anima_comeu_fantasma(window, g);

        //--------------------Pontuação--------------------//
        anima_pontuacao(window);

        //----------------------Desenha os itens no mapa------------------//
        anima_pill_and_bigpill(window);

        //--------------------Animação de início-----------------//
        if(modo == INICIO)
        {
            db<Window>(INF) << "INF: Animação de início\n";

            pac_2_sprite.setPosition(pacman->getPositionX(),pacman->getPositionY()); //pacman fechado
            window.draw(pac_2_sprite);

            ready_sprite.setPosition(Interface::convert_matriz_pixel_x(12),Interface::convert_matriz_pixel_y(17)); //ready
            window.draw(ready_sprite);

            debouncer++; //delay
            if( (!Traits<Timer>::active_timer && debouncer == 10 )
                || (Traits<Timer>::active_timer && debouncer == 1000) )
            {
                debouncer = 0;
                modo = RUNNING;
                key = "left"; //começa o jogo andando pra esquerda
            }
        }
        // ----------------- Animação do jogo pausado -------------- //
        else if ( modo == PAUSE){
            db<Window>(INF) << "INF: Animação de pausa\n";
            ready_sprite.setPosition(Interface::convert_matriz_pixel_x(12),Interface::convert_matriz_pixel_y(17)); //ready
            window.draw(ready_sprite);
        }
        //--------------------Começou o jogo----------------------//
        else
        {
            if(modo == MORREU)
            {
                db<Window>(INF) << "INF: Animação de morreu\n";

                //-----------------Animação do dead pacman----------------//
                debouncerMorreu++; //debouncer muda o sprite a cada 50 iterações
                if( (!Traits<Timer>::active_timer && debouncerMorreu <= 1*(dead+1) && dead < 11) 
                    || (Traits<Timer>::active_timer && debouncerMorreu <= 50*(dead+1) && dead < 11) )
                {
                    dead_pacman[dead].setPosition(pacman->getPositionX(),pacman->getPositionY());
                    window.draw(dead_pacman[dead]);
                    if( (!Traits<Timer>::active_timer && debouncerMorreu == 1*(dead+1)) 
                        || (Traits<Timer>::active_timer && debouncerMorreu == 50*(dead+1)) ) dead++;
                }
                //------------Acabou animação do dead pacman-------------//
                else if( (!Traits<Timer>::active_timer && debouncerMorreu >= 11) 
                        || (Traits<Timer>::active_timer && debouncerMorreu >= 550) )
                {
                    //------------------Animações de reinício-------------//
                    pacman->volta_posicao_inicial();

                    pac_2_sprite.setPosition(pacman->getPositionX(),pacman->getPositionY()); //pacman fechado
                    window.draw(pac_2_sprite);

                    ready_sprite.setPosition(Interface::convert_matriz_pixel_x(12),Interface::convert_matriz_pixel_y(17)); //ready
                    window.draw(ready_sprite);

                    debouncer++; //delay
                    if( (!Traits<Timer>::active_timer && debouncer == 10) 
                        || (Traits<Timer>::active_timer && debouncer == 1000) )
                    {
                        debouncer = 0;
                        debouncerMorreu = 0;
                        dead = 0;
                        modo = RUNNING;
                        key = "left"; //começa o jogo andando pra esquerda
                    }
                }
            }

            else if(modo == GAMEOVER)
            {
                db<Window>(INF) << "INF: Animação de gameover\n";

                //-----------------Animação do dead pacman----------------//
                anima_pacman_morte(window);
                //fica esperando usuário dar RESTART
            }

            else if(modo == WIN)
            {
                db<Window>(INF) << "INF: Animação de win\n";

                ready_sprite.setPosition(Interface::convert_matriz_pixel_x(12),Interface::convert_matriz_pixel_y(17));
                window.draw(ready_sprite);

                //fica esperando usuário dar RESTART
            }

            else if(modo == RESTART)
            {
                db<Window>(INF) << "INF: RESTART\n";
            }

            else
            {
                db<Window>(INF) << "INF: NORMAL\n";

                /*
                Atualiza as informações do fantasma na tela 
                */
                for(int g = 0; g < 4; g++) anima_fantasma(window,g);

                //-----------Faz pacman ficar abrindo e fechando a boca-----------//
                anima_pacman_boca(window);
            }
        }

        window.display();

        if(!Traits<Timer>::active_timer)
        {
            for (int i = 0; i < 20000000; i++){}

            Thread::yield();
        }

    }

    db<Window>(TRC) << "TRC: Acabou Window::run()\n";

    for(int g = 0; g < 4; g++) delete ghosts[g];
    delete pacman;

    Main::end = true; //encerra o jogo
    if(!Traits<Timer>::active_timer) Thread::yield();
}

void Window::anima_comeu_fantasma(sf::RenderWindow &janela, int g)
{
    if(ghostEat[g])
    {
        if(debouncerEatGhost[g] == 0) //pega a posição em que o fantasma foi comido
        {
            pos_ghost_morreu[g][0] = ghosts[g]->get_x_pixel();
            pos_ghost_morreu[g][1] = ghosts[g]->get_y_pixel();
        }

        debouncerEatGhost[g]++;

        score_200_sprite.setPosition(pos_ghost_morreu[g][0],pos_ghost_morreu[g][1]);
        janela.draw(score_200_sprite);

        if( (!Traits<Timer>::active_timer && debouncerEatGhost[g] == 15) 
            || (Traits<Timer>::active_timer && debouncerEatGhost[g] == 1000) )
        {
            debouncerEatGhost[g] = 0;
            ghostEat[g] = false;
            pos_ghost_morreu[g][0] = 0;
            pos_ghost_morreu[g][1] = 0;
        }
    }
}

// ---------------------- ANIMACOES -------------------//
void Window::anima_pill_and_bigpill(sf::RenderWindow &janela)
{
//----------------------Desenha os itens no mapa------------------//
    for(int i = 0; i < 31; i++)
    {
        for(int j = 0; j < 28; j++)
        {
            //------------Pill-----------//
            if(Interface::maze[i][j] == o)
            {
                pill_sprite.setPosition(Interface::convert_matriz_pixel_x(j),Interface::convert_matriz_pixel_y(i));
                janela.draw(pill_sprite);
            }
            //----------Big Pill----------//
            else if(Interface::maze[i][j] == O)
            {
                bigPill_sprite.setPosition(Interface::convert_matriz_pixel_x(j),Interface::convert_matriz_pixel_y(i));
                janela.draw(bigPill_sprite);
            }
        }
    }
}

void Window::anima_pontuacao(sf::RenderWindow &janela)
{
    //--------------------Pontuação--------------------//
    points = pacman->getPoints();

    numeros[points/1000].setPosition(480,40); //primeiro dígito
    janela.draw(numeros[points/1000]);

    numeros[(points%1000)/100].setPosition(490,40); //segundo dígito
    janela.draw(numeros[(points%1000)/100]);

    numeros[((points%1000)%100)/10].setPosition(500,40); //terceiro dígito
    janela.draw(numeros[((points%1000)%100)/10]);

    numeros[points%10].setPosition(510,40); //quarto dígito
    janela.draw(numeros[points%10]);
}

void Window::anima_pacman_morte(sf::RenderWindow &janela)
{
    //-----------------Animação do dead pacman----------------//
    debouncerMorreu++; //debouncer muda o sprite a cada 50 iterações
    if( (!Traits<Timer>::active_timer && debouncerMorreu <= 1*(dead+1) && dead < 11) 
        || (Traits<Timer>::active_timer && debouncerMorreu <= 50*(dead+1) && dead < 11) )
    {
        dead_pacman[dead].setPosition(pacman->getPositionX(),pacman->getPositionY());
        janela.draw(dead_pacman[dead]);
        if( (!Traits<Timer>::active_timer && debouncerMorreu == 1*(dead+1)) 
            || (Traits<Timer>::active_timer && debouncerMorreu == 50*(dead+1)) ) dead++;
    }
    //------------Acabou animação do dead pacman-------------//
    else if( (!Traits<Timer>::active_timer && debouncerMorreu >= 11) 
            || (Traits<Timer>::active_timer && debouncerMorreu >= 550) )
    {
        gameover_sprite.setPosition(Interface::convert_matriz_pixel_x(10),Interface::convert_matriz_pixel_y(17));
        janela.draw(gameover_sprite);
    }
}

void Window::anima_fantasma(sf::RenderWindow &janela, int g)
{
    //-------------------fantasma FUGITIVO----------------------//  
    if(ghosts[g]->get_estado_fantasma() == Ghost::FUGITIVO)
    {
        ghost_scared_0_sprite.setPosition(ghosts[g]->get_x_pixel(), ghosts[g]->get_y_pixel());
        janela.draw(ghost_scared_0_sprite);

        //---------fica em estado FUGITIVO por um período de tempo (6000)-----//
        debouncerGhost[g]++;
        //---------perto de acabar o tempo, fantasma começa a piscar---------//
        if( (!Traits<Timer>::active_timer && debouncerGhost[g] > 55 && debouncerGhost[g] < 80) 
            || (Traits<Timer>::active_timer && debouncerGhost[g] > 4000 && debouncerGhost[g] < 6000) )
        {
            if( (!Traits<Timer>::active_timer && debouncerGhost_aux[g] > 3) 
                || (Traits<Timer>::active_timer && debouncerGhost_aux[g] > 200) )
            {
                debouncerGhost_aux[g]++;
                ghost_scared_0_sprite.setPosition(ghosts[g]->get_x_pixel(), ghosts[g]->get_y_pixel());
                janela.draw(ghost_scared_0_sprite);

                if( (!Traits<Timer>::active_timer && debouncerGhost_aux[g] > 8) 
                    || (Traits<Timer>::active_timer && debouncerGhost_aux[g] > 300) ) debouncerGhost_aux[g] = 0;
            }
            else if( (!Traits<Timer>::active_timer && debouncerGhost_aux[g] <= 3) 
                    || (Traits<Timer>::active_timer && debouncerGhost_aux[g] <= 200) )
            {
                debouncerGhost_aux[g]++;
                ghost_scared_2_sprite.setPosition(ghosts[g]->get_x_pixel(), ghosts[g]->get_y_pixel());
                janela.draw(ghost_scared_2_sprite);
            }

        }
        //------------acabou o tempo, volta a ser ATACANTE-----------//
        else if( (!Traits<Timer>::active_timer && debouncerGhost[g] == 80) 
                || (Traits<Timer>::active_timer && debouncerGhost[g] == 6000) )
        {
            debouncerGhost[g] = 0;
            ghosts[g]->set_estado_fantasma(Ghost::ATACANTE);
        }
    }
    //-----------------fantasma ATACANTE ou MORTO---------------//           
    else
    {
        //---------------------fantasma ATACANTE-------------------//
        if(ghosts[g]->get_estado_fantasma() == Ghost::ATACANTE || ghosts[g]->get_estado_fantasma() == Ghost::SLEEP || 
            ghosts[g]->get_estado_fantasma() == Ghost::INIT || ghosts[g]->get_estado_fantasma() == Ghost::WAITING)
        {
            ghosts_sprites[g].setPosition(ghosts[g]->get_x_pixel(), ghosts[g]->get_y_pixel());
            janela.draw(ghosts_sprites[g]);
        }

        //-------------------olhos do fantasma-----------------//
        ghost_eyes[ghosts[g]->getDir()].setPosition(ghosts[g]->get_x_pixel(), ghosts[g]->get_y_pixel());
        janela.draw(ghost_eyes[ghosts[g]->getDir()]);
    }
}

void Window::anima_pacman_boca(sf::RenderWindow &janela)
{
    //-----------Faz pacman ficar abrindo e fechando a boca-----------//

    if(aumenta) debouncerWalkPacman++;
    else if(!aumenta) debouncerWalkPacman--;
    
    if( (!Traits<Timer>::active_timer && debouncerWalkPacman <= 1*(walk+1) && walk < 3 && aumenta) 
        || (Traits<Timer>::active_timer && debouncerWalkPacman <= 50*(walk+1) && walk < 3 && aumenta) )
    {
        //Configura a origem do sprite no centro dele (pra rotacionar)
        walk_pacman[walk].setOrigin(sf::Vector2f(walk_pacman[walk].getTexture()->getSize().x * 0.5,walk_pacman[walk].getTexture()->getSize().y * 0.5));
        walk_pacman[walk].setPosition(pacman->getPositionX() + 5,pacman->getPositionY() + 5); //5 pra posicionar certo
        walk_pacman[walk].setRotation(pacman->getAngle());
        janela.draw(walk_pacman[walk]);

        if( (!Traits<Timer>::active_timer && debouncerWalkPacman == 1*(walk+1)) 
            || (Traits<Timer>::active_timer && debouncerWalkPacman == 50*(walk+1)) ) walk++;
        if(walk == 3)
        {
            aumenta = false;
            walk--;
        }
    }
    else if( (!Traits<Timer>::active_timer && debouncerWalkPacman >= 1*walk && walk >= 0 && !aumenta) 
            || (Traits<Timer>::active_timer && debouncerWalkPacman >= 50*walk && walk >= 0 && !aumenta) )
    {
        walk_pacman[walk].setPosition(pacman->getPositionX() + 5,pacman->getPositionY() + 5); //5 pra posicionar certo
        walk_pacman[walk].setOrigin(sf::Vector2f(walk_pacman[walk].getTexture()->getSize().x * 0.5,walk_pacman[walk].getTexture()->getSize().y * 0.5));
        walk_pacman[walk].setRotation(pacman->getAngle());
        janela.draw(walk_pacman[walk]);

        if( (!Traits<Timer>::active_timer && debouncerWalkPacman == 1*walk) 
            || (Traits<Timer>::active_timer && debouncerWalkPacman == 50*walk) ) walk--;
        if(walk == -1)
        {
            aumenta = true;
            walk++;
        }
    }
}

void Window::anima_vidas(sf::RenderWindow &janela)
{
    //----------------------Vidas----------------------//
    for(int i = 0; i < pacman->getLife(); i++)
    {
        life_sprite.setPosition(480 + i*20,60);
        janela.draw(life_sprite);
    }
}

void Window::anima_fruta(sf::RenderWindow &janela)
{
    //----------------------Fruta----------------------//
    //posição da fruta embaixo da cadeia [17][13]
    if(pacman->getPill() == 20 || Interface::maze[17][13] != u)
    {
        cherry_sprite.setPosition(Interface::convert_matriz_pixel_x(13),Interface::convert_matriz_pixel_y(17));
        janela.draw(cherry_sprite);
        Interface::maze[17][13] = F;
    }
}

void Window::load_and_bind_textures()
{
    // Bind map textures    
    maze_tex.loadFromFile("sprites/maze/maze.png");
    maze_sprite.setTexture(maze_tex);
    maze_sprite.scale(2, 2);
    pill_tex.loadFromFile("sprites/maze/p-0.png");
    pill_sprite.setTexture(pill_tex);
    //pill_sprite.scale(2, 2); //adicionado, tava muito pequeno
    bigPill_tex.loadFromFile("sprites/maze/p-1.png");
    bigPill_sprite.setTexture(bigPill_tex);
    //bigPill_sprite.scale(2, 2); //adicionado, tava muito pequeno

    // Bind Pacman textures
    pac_0_tex.loadFromFile("sprites/pacman/0.png");
    pac_0_sprite.setTexture(pac_0_tex);
    //pac_0_sprite.scale(1.5, 1.5); //adicionado, tava muito pequeno
    pac_1_tex.loadFromFile("sprites/pacman/1.png");
    pac_1_sprite.setTexture(pac_1_tex);
    pac_2_tex.loadFromFile("sprites/pacman/2.png");
    pac_2_sprite.setTexture(pac_2_tex);
    dead_0_tex.loadFromFile("sprites/pacman/d-0.png");
    dead_0_sprite.setTexture(dead_0_tex);
    dead_1_tex.loadFromFile("sprites/pacman/d-1.png");
    dead_1_sprite.setTexture(dead_1_tex);
    dead_2_tex.loadFromFile("sprites/pacman/d-2.png");
    dead_2_sprite.setTexture(dead_2_tex);
    dead_3_tex.loadFromFile("sprites/pacman/d-3.png");
    dead_3_sprite.setTexture(dead_3_tex);
    dead_4_tex.loadFromFile("sprites/pacman/d-4.png");
    dead_4_sprite.setTexture(dead_4_tex);
    dead_5_tex.loadFromFile("sprites/pacman/d-5.png");
    dead_5_sprite.setTexture(dead_5_tex);
    dead_6_tex.loadFromFile("sprites/pacman/d-6.png");
    dead_6_sprite.setTexture(dead_6_tex);
    dead_7_tex.loadFromFile("sprites/pacman/d-7.png");
    dead_7_sprite.setTexture(dead_7_tex);
    dead_8_tex.loadFromFile("sprites/pacman/d-8.png");
    dead_8_sprite.setTexture(dead_8_tex);
    dead_9_tex.loadFromFile("sprites/pacman/d-9.png");
    dead_9_sprite.setTexture(dead_9_tex);
    dead_10_tex.loadFromFile("sprites/pacman/d-10.png");
    dead_10_sprite.setTexture(dead_10_tex);

    // Bind ghost textures
    ghost_r_0_tex.loadFromFile("sprites/ghosts/r-0.png");
    ghost_r_0_sprite.setTexture(ghost_r_0_tex);
    ghost_r_1_tex.loadFromFile("sprites/ghosts/r-1.png");
    ghost_r_1_sprite.setTexture(ghost_r_1_tex);
    ghost_p_0_tex.loadFromFile("sprites/ghosts/p-0.png");
    ghost_p_0_sprite.setTexture(ghost_p_0_tex);
    ghost_p_1_tex.loadFromFile("sprites/ghosts/p-1.png");
    ghost_p_1_sprite.setTexture(ghost_p_1_tex);
    ghost_b_0_tex.loadFromFile("sprites/ghosts/b-0.png");
    ghost_b_0_sprite.setTexture(ghost_b_0_tex);
    ghost_b_1_tex.loadFromFile("sprites/ghosts/b-1.png");
    ghost_b_1_sprite.setTexture(ghost_b_1_tex);
    ghost_y_0_tex.loadFromFile("sprites/ghosts/y-0.png");
    ghost_y_0_sprite.setTexture(ghost_y_0_tex);
    ghost_y_1_tex.loadFromFile("sprites/ghosts/y-1.png");
    ghost_y_1_sprite.setTexture(ghost_y_1_tex);
    ghost_scared_0_tex.loadFromFile("sprites/ghosts/s-0.png");
    ghost_scared_0_sprite.setTexture(ghost_scared_0_tex);
    ghost_scared_1_tex.loadFromFile("sprites/ghosts/s-1.png");
    ghost_scared_1_sprite.setTexture(ghost_scared_1_tex);
    ghost_scared_2_tex.loadFromFile("sprites/ghosts/s-2.png");
    ghost_scared_2_sprite.setTexture(ghost_scared_2_tex);
    ghost_scared_3_tex.loadFromFile("sprites/ghosts/s-3.png");
    ghost_scared_3_sprite.setTexture(ghost_scared_3_tex);

     // Bind ghost eye textures
    eye_up_tex.loadFromFile("sprites/eyes/up.png");
    eye_up_sprite.setTexture(eye_up_tex);
    eye_right_tex.loadFromFile("sprites/eyes/right.png");
    eye_right_sprite.setTexture(eye_right_tex);
    eye_down_tex.loadFromFile("sprites/eyes/down.png");
    eye_down_sprite.setTexture(eye_down_tex);
    eye_left_tex.loadFromFile("sprites/eyes/left.png");
    eye_left_sprite.setTexture(eye_left_tex);

    // Bind UI textures
    num_0_tex.loadFromFile("sprites/ui/0.png");
    num_0_sprite.setTexture(num_0_tex);
    num_1_tex.loadFromFile("sprites/ui/1.png");
    num_1_sprite.setTexture(num_1_tex);
    num_2_tex.loadFromFile("sprites/ui/2.png");
    num_2_sprite.setTexture(num_2_tex);
    num_3_tex.loadFromFile("sprites/ui/3.png");
    num_3_sprite.setTexture(num_3_tex);
    num_4_tex.loadFromFile("sprites/ui/4.png");
    num_4_sprite.setTexture(num_4_tex);
    num_5_tex.loadFromFile("sprites/ui/5.png");
    num_5_sprite.setTexture(num_5_tex);
    num_6_tex.loadFromFile("sprites/ui/6.png");
    num_6_sprite.setTexture(num_6_tex);
    num_7_tex.loadFromFile("sprites/ui/7.png");
    num_7_sprite.setTexture(num_7_tex);
    num_8_tex.loadFromFile("sprites/ui/8.png");
    num_8_sprite.setTexture(num_8_tex);
    num_9_tex.loadFromFile("sprites/ui/9.png");
    num_9_sprite.setTexture(num_9_tex);
    score_200_tex.loadFromFile("sprites/ui/200.png");
    score_200_sprite.setTexture(score_200_tex);
    score_400_tex.loadFromFile("sprites/ui/400.png");
    score_400_sprite.setTexture(score_400_tex);
    score_800_tex.loadFromFile("sprites/ui/800.png");
    score_800_sprite.setTexture(score_800_tex);
    score_1600_tex.loadFromFile("sprites/ui/1600.png");
    score_1600_sprite.setTexture(score_1600_tex);
    ready_tex.loadFromFile("sprites/ui/ready.png");
    ready_sprite.setTexture(ready_tex);
    ready_sprite.scale(1.5,1.5); //adicionado
    gameover_tex.loadFromFile("sprites/ui/gameover.png");
    gameover_sprite.setTexture(gameover_tex);
    gameover_sprite.scale(1.5,1.5); //adicionado
    life_tex.loadFromFile("sprites/ui/life.png");
    life_sprite.setTexture(life_tex);
    score_tex.loadFromFile("sprites/ui/score_tex.png");
    score_sprite.setTexture(score_tex);
    score_sprite.scale(1.5,1.5); //adicionado
    high_tex.loadFromFile("sprites/ui/high.png");
    high_sprite.setTexture(high_tex);
    score_100_tex.loadFromFile("sprites/ui/100.png");
    score_100_sprite.setTexture(score_100_tex);
    score_300_tex.loadFromFile("sprites/ui/300.png");
    score_300_sprite.setTexture(score_300_tex);

    // Fruit textures
    cherry_tex.loadFromFile("sprites/fruits/cherry.png");
    cherry_sprite.setTexture(cherry_tex);
    cherry_sprite.scale(0.7,0.7); //adicionado
    strawberry_tex.loadFromFile("sprites/fruits/strawberry.png");
    strawberry_sprite.setTexture(strawberry_tex);
}

__END_API
