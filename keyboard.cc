#include "keyboard.h"

__BEGIN_API

bool Keyboard::left = false;
bool Keyboard::right = false;
bool Keyboard::down = false;
bool Keyboard::up = false;
bool Keyboard::paused = false;
bool Keyboard::reset = false;

std::string Keyboard::buffer_teclado;

Keyboard::Keyboard(){Keyboard::buffer_teclado = " ";}
Keyboard::~Keyboard(){}

#define DELAY 20000

/*
    Obtem o evento da tecla pressionada
    -> Após recolher o evento do pressionamento
    da trecla, limpo o buffer de evento
*/
static std::string get_tecla_pressionada(void)
{
    std::string aux = Keyboard::buffer_teclado;
    Keyboard::buffer_teclado = " ";
    return aux;
}

/*
    Função responsavel por analisar os eventos
    do teclado
    -> Sua captura é armazenada no buffer_teclado
*/
void Keyboard::run()
{
    // Verificar se a janela de eventos está acessavel
    if (!Window::janela || !Window::eventos){
        return;
    }

    // Obter os eventos da janela
    if (Window::janela->pollEvent(*Window::eventos))
    {
        switch (Window::eventos->type) {
        // Chamada para fechar o jogo
        case sf::Event::Closed:
            Window::janela->close();
            break;
        
        // Evento de tecla precionada
        case sf::Event::KeyPressed:
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                db<Keyboard>(INF) << "INF: Keyboard esquerda!\n";
                buffer_teclado = "left";
                Window::key = "left";
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                db<Keyboard>(INF) << "INF: Keyboard direita!\n";
                buffer_teclado = "right";
                Window::key = "right";
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                db<Keyboard>(INF) << "INF: Keyboard para baixo!\n";
                buffer_teclado = "down";
                Window::key = "down";
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                db<Keyboard>(INF) << "INF: Keyboard para cima!\n";
                buffer_teclado = "up";
                Window::key = "up";
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) { //sai do jogo
                db<Keyboard>(INF) << "INF: Quit\n";
                buffer_teclado = "close";
                //Window::janela->close();
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::P)) { //pausa
                db<Keyboard>(INF) << "INF: Pause\n";
                buffer_teclado = "pause";
                Window::key = "pause";
            } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)) { //reinicia
                db<Keyboard>(INF) << "INF: Restart\n";
                buffer_teclado = "restart";
                Window::key = "restart";
            }
            break;
        }
    }

    // delay para a fução de eventos não colapsar ( sério )
    for (int i = 0; i < DELAY; i++){}

    db<Keyboard>(TRC) << "TRC: Keyboard run()\n";

    if(Window::key == "left")
    {
        db<Keyboard>(INF) << "INF: left\n";
        left = true;
        right = false;
        down = false;
        up = false;
    }
    else if(Window::key == "right")
    {
        db<Keyboard>(INF) << "INF: right\n";
        left = false;
        right = true;
        down = false;
        up = false;
    }
    else if(Window::key == "down")
    {
        db<Keyboard>(INF) << "INF: down\n";
        left = false;
        right = false;
        down = true;
        up = false;
    }
    else if(Window::key == "up")
    {
        db<Keyboard>(INF) << "INF: up\n";
        left = false;
        right = false;
        down = false;
        up = true;
    }
    else if(Window::key == "pause")
    {
        db<Keyboard>(INF) << "INF: pause\n";
        if(Window::modo == Window::PAUSE) Window::modo = Window::RUNNING;
        else Window::modo = Window::PAUSE;

        Window::key = " ";
    }

    else if(Window::key == "restart")
    {
        db<Keyboard>(INF) << "INF: restart\n";
        Window::modo = Window::RESTART;

        Window::key = " ";
    }

}

__END_API