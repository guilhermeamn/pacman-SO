#ifndef main_class_h
#define main_class_h

#include <iostream>
#include "srv/cpu.h"
#include "srv/traits.h"
#include "srv/thread.h"
#include <unistd.h>
#include "window.h"
#include "srv/semaphore.h"

__BEGIN_API

class Main
{
public:
    Main() {
    }

    static void run(void * name)
    {
        std::cout << (char *) name << ": inicio\n";

        std::string cenario_name = "Cenario";
        std::string pacman_name = "Pacman";
        std::string ghosts_name[4] = {"Ghost0","Ghost1","Ghost2","Ghost3"};
        std::string keyboard_name = "Keyboard";

        keyboard = new Thread(key, (char *)keyboard_name.data(),0);
        cenario = new Thread(window, (char *)cenario_name.data(), 1);
        pacman = new Thread(pac, (char *)pacman_name.data(), 2);
        ghosts[0] = new Thread(ghost, (char *)ghosts_name[0].data(), 3, 0);
        ghosts[1] = new Thread(ghost, (char *)ghosts_name[1].data(), 4, 1);
        ghosts[2] = new Thread(ghost, (char *)ghosts_name[2].data(), 5, 2);
        ghosts[3] = new Thread(ghost, (char *)ghosts_name[3].data(), 5, 3);

        sem = new Semaphore();

        std::cout << "Esperando keyboard..." << std::endl;
        keyboard->join();
        std::cout << "keyboard voltou" << std::endl;

        std::cout << "Esperando cenario..." << std::endl;
        cenario->join();
        std::cout << "cenario voltou" << std::endl;

        std::cout << "Esperando pacman..." << std::endl;
        pacman->join();
        std::cout << "pacman voltou" << std::endl;

        std::cout << "Esperando ghosts[0]..." << std::endl;
        ghosts[0]->join();
        std::cout << "ghosts[0] voltou" << std::endl;

        std::cout << "Esperando ghosts[1]..." << std::endl;
        ghosts[1]->join();
        std::cout << "ghosts[1] voltou" << std::endl;

        std::cout << "Esperando ghosts[2]..." << std::endl;
        ghosts[2]->join();
        std::cout << "ghosts[2] voltou" << std::endl;

        std::cout << "Esperando ghosts[3]..." << std::endl;
        ghosts[3]->join();
        std::cout << "ghosts[3] voltou" << std::endl;

        std::cout << "main: fim\n";

        delete sem;

        delete pacman;
        delete ghosts[0];
        delete ghosts[1];
        delete ghosts[2];
        delete ghosts[3];
        delete keyboard;
        delete cenario;

    }

    ~Main() {}

private:
    /*
        Estrutura da Thread que irá chamar as
        mecanicas do fantasma
    */
    static void ghost(char *name, int id, int posicao)
    {
        while(!endGhosts[posicao])
        {
            w.runGhost(posicao);
            if(!Traits<Timer>::active_timer) Thread::yield();
        }

        ghosts[posicao]->thread_exit(id);
    }

    static void pac(char *name, int id)
    {
        while(!endPacman)
        {
            w.runPacman();
            if(!Traits<Timer>::active_timer) Thread::yield();
        }
        pacman->thread_exit(id);
    }

    static void window(char *name, int id)
    {
        while(!end)
        {
            w.run();
        }
        cenario->thread_exit(id);
    }

    static void key(char *name, int id)
    {
        while(!end)
        {
            Keyboard::run();
            if(!Traits<Timer>::active_timer) Thread::yield();
        }
        keyboard->thread_exit(id);
    }

    private:
        static Thread *cenario;
        static Thread *pacman;
        static Thread *ghosts[4];
        static Thread *keyboard;
        static Window w;

    public:
        static bool end;
        static bool endPacman;
        static bool endGhosts[4];
        static bool endKeyboard;
        static Semaphore *sem;
};

__END_API

#endif
