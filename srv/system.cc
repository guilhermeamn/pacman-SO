#include "system.h"

__BEGIN_API

void System::init(void (*main)(void *)) {
    db<System>(TRC) << "TRC: System::init\n";
    setvbuf(stdout,0,_IONBF,0);
    Thread::init(main);
}

__END_API