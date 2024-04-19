#include "window.h"
#include "main_class.h"
#include "srv/system.h"

__USING_API

int main(void)
{
    srand(time(NULL));
    System::init(&Main::run);

    return 0;
}
