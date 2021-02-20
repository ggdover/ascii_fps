#include "rendering.h"
#include "constants.h"
#include <cassert> // assert
#include <ncurses.h> // move, printw

// PARAMETERS:
// x [in]          = Which column (in x-axis) that we are currently shading
// ceiling [in]    = y-coordinate at which ceiling starts (from the wall).
//                   Can also be seen as the lowest y-coordinate that is part of the ceiling
// floor [in]      = y-coordinate at which floor starts (from the wall).
//                   Can also be seen as the highest y-coordinate that is part of the floor
// distanceToWall [in] = Distance to wall for the column determined by parameter 'x'
// screen [in/out]     = Variable that holds the characters that will be printed to represent
//                       our field-of-view. Every call to this function fills up one column
//                       in this variable. Which column is determined by the parameter 'x'
void ascii_shade_column(int x, int ceiling, int floor, float distanceToWall, std::string &screen)
{
    // Character that will be rendered, will differ to represent
    // different shade depending on distance/depth of vision.
    char shade = ' ';

    // Iterating from bottom to top, all squares in the column we are currently rendering
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        if (y < ceiling)
        {
            // This pixel is part of the ceiling
            screen[y * SCREEN_WIDTH + x] = ' ';
        }
        else if (y > ceiling && y <= floor)
        {
            // Get shade based on current distance
            // 1. Get precentage of how far the distance is
            //    1.0f (100%) means that distance is MAX_DEPTH
            //    (The max of what we can see
            float sight_distance = distanceToWall / MAX_DEPTH;
            // 2. Figure out which level of shade based on the precentage
            std::string shades = "@%#*=- "; // Characters that make up the total grayscale of shade.
                                            // Goes from brightest to darkest, or to be more exact, from shade
                                            // at closest distance to wall to shade at longest or infinite distance to wall.

            assert(shades.length() > 0); // Otherwise, shade_index on next line could be negative
            int shade_index = sight_distance * (shades.length() - 1);
            shade = shades[shade_index];

            // This pixel is part of the wall (neither ceiling or floor)
            screen[y * SCREEN_WIDTH + x] = shade;
        }
        else
        {
            // This pixel is part of the floor

            // I don't understand this code fully yet, just copied from the youtube video I follow
            // Will add explanation of this code in the future (unless I get lazy..)
            float b = 1.0f - (((float)y - SCREEN_HEIGHT / 2.0f) / ((float)SCREEN_HEIGHT / 2.0f));
            if (b < 0.4)
                shade = '+';
            else
                shade = '.';

            screen[y * SCREEN_WIDTH + x] = shade;
        }
    }
}

// Draw the the ascii characters we have stored in the variable 'screen'
// with each call to 'ascii_shaded_column'.
// - Also does some last touches, adding newlines for example
//   before doing the actual print
void ascii_draw(std::string &screen)
{
    // Sacrifice the right most column for newlines
    for (int i = 0; i < SCREEN_HEIGHT; ++i)
    {
        int last_pixel = i * (SCREEN_WIDTH) + (SCREEN_WIDTH-1); // Get last pixel/char of row
        screen[last_pixel] = '\n';
    }
    // We only needed this when screen was a char-array, now its std::string instead
    // testing with having it commented out before I remove.
    //screen[SCREEN_WIDTH*SCREEN_HEIGHT - 1] = '\0';

    // Move to top-left corner of terminal before we print/draw/render.
    move(0,0);
    // This is essentially our function call for the "Drawing/Rendering" to happen
    printw("%s\n", screen.c_str());
}
