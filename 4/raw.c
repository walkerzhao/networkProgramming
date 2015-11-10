#include <termios.h>
#include <unistd.h>
#include "raw.h"

/* See raw.h for usage information */

static struct termios oldterm;

/* Returns -1 on error, 0 on success */
int raw_mode (void)
{
        struct termios term;

        if (tcgetattr(STDIN_FILENO, &term) != 0) return -1;
    
        oldterm = term;     
        term.c_lflag &= ~(ECHO);    /* Turn off echoing of typed charaters */
        term.c_lflag &= ~(ICANON);  /* Turn off line-based input */
        term.c_cc[VMIN] = 1;
        term.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSADRAIN, &term);

        return 0;
}

void cooked_mode (void)    
{   
        tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);
}
