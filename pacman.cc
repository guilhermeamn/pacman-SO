#include "pacman.h"

__BEGIN_API

Pacman::Pacman()
{
    db<Pacman>(TRC) << "TRC: Pacman()\n";

    positionX = 220;
    positionY = 365;
    posX_maze = 365/15.5357;
    posY_maze = 220/15.6452;
    angle = 0.f;
    pill = 0;
    life = 3;
    bigPill = 0;
    fruit = 0;
    points = 0;
    debouncerPacman = 0;
}

Pacman::Pacman(int x, int y)
{
    db<Pacman>(TRC) << "TRC: Pacman(int,int)\n";

    positionX = x;
    positionY = y;
    posX_maze = y/15.5357;
    posY_maze = x/15.6452;
    angle = 0.f;
    pill = 0;
    life = 3;
    bigPill = 0;
    fruit = 0;
    points = 0;
    debouncerPacman = 0;
}

/*
 * Método principal da thread Pacman
 * @return: 0 -> estado normal
 *          1 -> precisa alterar o estado do fantasma para FUGITIVO
 */
int Pacman::run()
{
    db<Pacman>(TRC) << "TRC: runPacman\n";

    //----------240 pills e 4 big pills no jogo, se pacman comeu todas, ganhou------//
    if(pill == 240 && bigPill == 4) Window::modo = Window::WIN;

    int x_matriz = posX_maze;
    int y_matriz = posY_maze;

    int retorno = 0;

    if(Window::modo == Window::RUNNING)
    {
        //-------------------Botão Esquerda-------------------//
        if(Keyboard::left && y_matriz > 0)
        {
            db<Pacman>(INF) << "INF: Pacman left\n";
            retorno = check(0.f,-1,x_matriz,y_matriz,1);
        }

        //-------------------Botão Direta-------------------//
        else if(Keyboard::right && y_matriz < 27)
        { 
            db<Pacman>(INF) << "INF: Pacman right\n";
            retorno = check(180.f,1,x_matriz,y_matriz,1);
        }

        //-------------------Botão Cima-------------------//
        else if(Keyboard::up && x_matriz > 0)
        {
            db<Pacman>(INF) << "INF: Pacman up\n";
            retorno = check(90.f,-1,x_matriz,y_matriz,0);
        }

        //-------------------Botão Baixo-------------------//
        else if(Keyboard::down && x_matriz < 30) 
        {
            db<Pacman>(INF) << "INF: Pacman down\n";
            retorno = check(270.f,1,x_matriz,y_matriz,0);
        }
    }

    atualizaPontos();

    return retorno;
}

/* 
 * Cada pill vale 10 pontos, big pill 50 pontos, cereja 100 pontos
 */
void Pacman::atualizaPontos()
{
    points = (pill * 10) + (bigPill * 50) + (fruit * 100);
}

/*
 * Reseta configurações de posição
 */
void Pacman::volta_posicao_inicial()
{
    positionX = 220;
    positionY = 365;
    posX_maze = 365/15.5357;
    posY_maze = 220/15.6452;
}

/*
 * Checa posições no eixo x ou y
 * @param: angulo -> ângulo para rotação do pacman
 *         dir -> 1 para direita/baixo, -1 para esquerda/cima
 *         x_matriz, y_matriz -> posição atual do pacman na matriz
 *         eixo -> 0 para x, 1 para y
 * @return: 0 -> estado normal
 *          1 -> precisa alterar o estado do fantasma para FUGITIVO
 */
int Pacman::check(float angulo, int dir, int x_matriz, int y_matriz, int eixo)
{
    angle = angulo; //configura rotação

    if(eixo == 0) x_matriz += dir;
    else if(eixo == 1) y_matriz += dir;

    //---------------Se posição não for parede, nem portal dos fantasmas----------//
    if(Interface::maze[x_matriz][y_matriz] != W && Interface::maze[x_matriz][y_matriz] != G)
    {
        //---------------Se posição for portal----------//
        if(Interface::maze[x_matriz][y_matriz] == P && eixo == 1) //só checa portal no eixo x (y da matriz)
        {
            db<Pacman>(INF) << "INF: Pacman [P]\n";

            //---------Se tiver no portal da esquerda, teleporta pra direita---//
            if(y_matriz == 0) y_matriz = 26;
            //---------Se tiver no portal da direita, teleporta pra esquerda---//
            else if(y_matriz == 27) y_matriz = 1;
        }
        //---------------Se posição for pill----------//
        else if(Interface::maze[x_matriz][y_matriz] == o)
        {
            db<Pacman>(INF) << "INF: Pacman [o]\n";
            Interface::maze[x_matriz][y_matriz] = e; //transforma pra eaten pill
            pill++; //adiciona no número do pacman
        }
        //---------------Se posição for big pill----------//
        else if(Interface::maze[x_matriz][y_matriz] == O)
        {
            db<Pacman>(INF) << "INF: Pacman [O]\n";
            Interface::maze[x_matriz][y_matriz] = E; //transforma pra eaten big pill
            bigPill++;
            return 1;
        }
        //----------------Se posição for fruit------------//
        else if(Interface::maze[x_matriz][y_matriz] == F)
        {
            db<Pacman>(INF) << "INF: Pacman [F]\n";
            Interface::maze[x_matriz][y_matriz] = u;
            Window::fruitEat = true;
            fruit++;
        }
        
        //------------move o pacman---------------//
        if(eixo == 0) posX_maze = x_matriz;
        else if(eixo == 1) posY_maze = y_matriz;

        //-------atualiza a posição em pixels-----//
        positionX = Interface::convert_matriz_pixel_x(posY_maze);
        positionY = Interface::convert_matriz_pixel_y(posX_maze);
    }

    return 0;
}

/*
 * Reseta os parâmetros iniciais de pacman, quando o usuário aperta restart
 */
void Pacman::reset()
{
    volta_posicao_inicial();
    angle = 0.f;
    pill = 0;
    life = 3;
    bigPill = 0;
    fruit = 0;
    points = 0;
}

void Pacman::setPositionX(int x)
{
    positionX = x;
}

void Pacman::setPositionY(int y)
{
    positionY = y;
}

void Pacman::setPosX_maze(int x)
{
    posX_maze = x;
}

void Pacman::setPosY_maze(int y)
{
    posY_maze = y;
}

void Pacman::setAngle(float a)
{
    angle = a;
}

void Pacman::setPill(int p)
{
    pill = p;
}

void Pacman::setLife(int l)
{
    life = l;
}

void Pacman::setBigPill(int b)
{
    bigPill = b;
}

void Pacman::setFruit(int f)
{
    fruit = f;
}

void Pacman::setPoints(int p)
{
    points = p;
}

void Pacman::set_debouncerPacman(int d)
{
    debouncerPacman = d;
}

int Pacman::getPositionX() const
{
    return positionX;
}

int Pacman::getPositionY() const
{
    return positionY;
}

int Pacman::getPosX_maze() const
{
    return posX_maze;
}

int Pacman::getPosY_maze() const
{
    return posY_maze;
}

float Pacman::getAngle() const
{
    return angle;
}

int Pacman::getPill() const
{
    return pill;
}

int Pacman::getLife() const
{
    return life;
}

int Pacman::getBigPill() const
{
    return bigPill;
}

int Pacman::getFruit() const
{
    return fruit;
}

int Pacman::getPoints() const
{
    return points;
}

int Pacman::get_debouncerPacman() const
{
    return debouncerPacman;
}

Pacman::~Pacman()
{
    db<Pacman>(TRC) << "TRC: ~Pacman()\n";
}

__END_API