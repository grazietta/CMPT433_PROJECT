#include "joystick.h"
#include "pot.h"
#include "clap.h"

int main()
{
    /*initialize*/
    joystick_init();
    pot_init();
    clap_init();

    /*cleanup*/
    joystick_cleanup();
    pot_cleanup();
    clap_cleanup();

    return 0;
}
