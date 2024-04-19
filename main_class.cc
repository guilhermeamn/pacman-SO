#include "main_class.h"

__BEGIN_API

Thread *Main::cenario;
Thread *Main::pacman;
Thread *Main::ghosts[4];
Thread *Main::keyboard;
Window Main::w;
bool Main::end = false;
bool Main::endPacman = false;
bool Main::endGhosts[4] = {false,false,false,false};
bool Main::endKeyboard = false;
Semaphore *Main::sem;

__END_API