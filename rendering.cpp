#include "rendering.h"
#include "constants.h"
#include <cassert> // assert
#include <ncurses.h> // move, printw
#include <vector> // vector

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

    // Iterating top to bottom, all squares in the column we are currently rendering
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

            // precentage of how far down on screen our current y-coordinate is.
            // 1.0f means its in the very middle of screen, 0.0f means its at the very bottom.
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

    // Move to top-left corner of terminal before we print/draw/render.
    move(0,0);
    // This is essentially our function call for the "Drawing/Rendering" to happen
    printw("%s", screen.c_str());
}

void init_colors()
{
    /* initialize colors */

    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    if (can_change_color() == FALSE) {
        endwin();
        printf("Your terminal does not support color change\n");
        exit(1);
    }

    start_color();

    // init_color
    // param 1     : color name
    // param 2, 3, 4 : rgb content min = 0, max = 1000 */

    // ncurses has colors from 1 to 8 already predefined,
    // so lets start from 9 (eventhough we could just override
    // the predefined 1 to 8 colors with init_color call probably)
    // but, anyway, fuck it...

    // foreground color (only because its neccessary for init_pair)
    assert(init_color(9, 255, 255, 255) == OK);

    // Wall background colors/shades
    // (From brightest to darkest)
    assert(init_color(10, 584, 584, 686) == OK);
    assert(init_color(11, 529, 529, 623) == OK);
    assert(init_color(12, 474, 474, 561) == OK);
    assert(init_color(13, 423, 423, 498) == OK);
    assert(init_color(14, 368, 368, 435) == OK);
    assert(init_color(15, 318, 318, 372) == OK);
    assert(init_color(16, 263, 263, 310) == OK);
    assert(init_color(17, 212, 212, 247) == OK);
    assert(init_color(18, 157, 157, 184) == OK);
    assert(init_color(19, 104, 104, 121) == OK);
    assert(init_color(20,  50,  50,  59) == OK);

    // Floor and Ceiling background colors/shades
    // (From brightest to darkest)
    assert(init_color(30, 498, 165, 98) == OK);
    assert(init_color(31, 435, 145, 86) == OK);
    assert(init_color(32, 372, 122, 74) == OK);
    assert(init_color(33, 310, 102, 59) == OK);
    assert(init_color(34, 247,  82, 47) == OK);
    assert(init_color(35, 184,  59, 35) == OK);
    assert(init_color(36, 122,  39, 24) == OK);
    assert(init_color(37,  47,  16,  8) == OK);

    // Need to setup pairs before we can apply them
    // - Creates a pair out of a foreground (color of text) and
    //   background (color of text background) color

    // Wall shades
    init_pair(10, 9, 10);
    init_pair(11, 9, 11);
    init_pair(12, 9, 12);
    init_pair(13, 9, 13);
    init_pair(14, 9, 14);
    init_pair(15, 9, 15);
    init_pair(16, 9, 16);
    init_pair(17, 9, 17);
    init_pair(18, 9, 18);
    init_pair(19, 9, 19);
    init_pair(20, 9, 20);

    // Floor/Ceiling shades
    init_pair(30, 9, 30);
    init_pair(31, 9, 31);
    init_pair(32, 9, 32);
    init_pair(33, 9, 33);
    init_pair(34, 9, 34);
    init_pair(35, 9, 35);
    init_pair(36, 9, 36);
    init_pair(37, 9, 37);
}

// PARAMETERS:
// x [in]          = Which column (in x-axis) that we are currently shading
// ceiling [in]    = y-coordinate at which ceiling starts (from the wall).
//                   Can also be seen as the lowest y-coordinate that is part of the ceiling
// floor [in]      = y-coordinate at which floor starts (from the wall).
//                   Can also be seen as the highest y-coordinate that is part of the floor
// distanceToWall [in] = Distance to wall for the column determined by parameter 'x'
void colored_draw_wall_column(int x, int ceiling, int floor, float distanceToWall)
{
    // Get shade based on current distance
    // 1. Get precentage of how far the distance is
    //    1.0f (100%) means that distance is MAX_DEPTH
    //    (The max of what we can see
    float sight_distance = distanceToWall / MAX_DEPTH;
    int wall_length = floor - ceiling;

    if (sight_distance < 0.05f)
    {
        attron(COLOR_PAIR(10));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(10));
    }
    else if (sight_distance < 0.1f)
    {
        attron(COLOR_PAIR(11));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(11));
    }
    else if (sight_distance < 0.15f)
    {
        attron(COLOR_PAIR(12));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(12));
    }
    else if (sight_distance < 0.2f)
    {
        attron(COLOR_PAIR(13));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(13));
    }
    else if (sight_distance < 0.25f)
    {
        attron(COLOR_PAIR(14));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(14));
    }
    else if (sight_distance < 0.3f)
    {
        attron(COLOR_PAIR(15));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(15));
    }
    else if (sight_distance < 0.35f)
    {
        attron(COLOR_PAIR(16));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(16));
    }
    else if (sight_distance < 0.4f)
    {
        attron(COLOR_PAIR(17));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(17));
    }
    else if (sight_distance < 0.45f)
    {
        attron(COLOR_PAIR(18));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(18));
    }
    else if (sight_distance < 0.5f)
    {
        attron(COLOR_PAIR(19));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(19));
    }
    else if (sight_distance < 0.6f)
    {
        attron(COLOR_PAIR(20));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(20));
    }
    else
    {
        // Make the shade for longest distance to wall the same
        // as the longest distance/darkest shade for ceiling/floor
        // so that the most distant wall just blends into the background.
        attron(COLOR_PAIR(37));
        mvvline(ceiling, x, ' ', wall_length);
        attron(COLOR_PAIR(37));
    }
}

void colored_draw_ceiling_and_floor()
{
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        // Precentage value of where the current pixel is
        // ( This is to make sure where shadings start/end
        //   scales in accordance to change in SCREEN_WIDTH/HEIGHT
        // - 0.0f means top of screen, 1.0f means bottom of screen
        float y_prec = y / (float)SCREEN_HEIGHT;

        if (y_prec < 0.02f || y_prec > 0.98f)
        {
            attron(COLOR_PAIR(30));
            mvhline(y, 0, ' ', SCREEN_WIDTH);
            attron(COLOR_PAIR(30));
        }
        else if (y_prec < 0.05f || y_prec > 0.95f)
        {
            attron(COLOR_PAIR(31));
            mvhline(y, 0, ' ', SCREEN_WIDTH);
            attron(COLOR_PAIR(31));
        }
        else if (y_prec < 0.08f || y_prec > 0.92f)
        {
            attron(COLOR_PAIR(32));
            mvhline(y, 0, ' ', SCREEN_WIDTH);
            attron(COLOR_PAIR(32));
        }
        else if (y_prec < 0.13f || y_prec > 0.87f)
        {
            attron(COLOR_PAIR(33));
            mvhline(y, 0, ' ', SCREEN_WIDTH);
            attron(COLOR_PAIR(33));
        }
        else if (y_prec < 0.16f || y_prec > 0.84f)
        {
            attron(COLOR_PAIR(34));
            mvhline(y, 0, ' ', SCREEN_WIDTH);
            attron(COLOR_PAIR(34));
        }
        else if (y_prec < 0.20f || y_prec > 0.80f)
        {
            attron(COLOR_PAIR(35));
            mvhline(y, 0, ' ', SCREEN_WIDTH);
            attron(COLOR_PAIR(35));
        }
        else if (y_prec < 0.25f || y_prec > 0.75f)
        {
            attron(COLOR_PAIR(36));
            mvhline(y, 0, ' ', SCREEN_WIDTH);
            attron(COLOR_PAIR(36));
        }
        else
        {
            attron(COLOR_PAIR(37));
            mvhline(y, 0, ' ', SCREEN_WIDTH);
            attron(COLOR_PAIR(37));
        }
    }
}
