// input.h - This module is created to give us
//           a way to read key inputs without
//           it being blocked or buffered.
//           - Not Blocked meaning it won't wait
//             for input before proceeding
//           - Not Buffered meaning we don't have
//             to hit enter for it to proceed.

#include <ncurses.h>

void init_input(void);
int kbhit(void);

// Functions to call, but that exist directly in 'ncurses.h'

// Get last pressed character/key
// int getch()

// Print (To be used instead of printf, because output looks messed up with printf)
// printw()
