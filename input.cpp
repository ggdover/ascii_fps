// Taken from: https://stackoverflow.com/questions/4025891/create-a-function-to-check-for-key-press-in-unix-using-ncurses

//#include <ncurses.h>
//#include <unistd.h>  /* only for sleep() */
#include <cstdlib>
#include "input.h"

static bool initInputHasBeenCalled = false;

void init_input(void)
{
    initscr();

    cbreak();
    noecho(); // noecho means, Don't print character when its typed
    nodelay(stdscr, TRUE);
    scrollok(stdscr, FALSE); // scrollok(.., FALSE) means, Dont't scroll screen if cursor moves past the last line. (According to what I've read online, on man pages I think..)

    initInputHasBeenCalled = true;
}

int kbhit(void)
{
    if (!initInputHasBeenCalled)
    {
        printf("ERROR init_input has not been called!!!\n");
        exit(0);
    }

    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

#ifdef false
// -------------------------------------------------
// ----------------- Example Code  -----------------
// -------------------------------------------------

if (kbhit()) {
    printw("Key pressed! It was: %d\n", getch());
    refresh();
} else {
    printw("No key pressed yet...\n");
    refresh();
    sleep(1);
}
#endif
