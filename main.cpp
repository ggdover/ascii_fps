//#include <stdio.h>
#include <string>
#include <cmath>
#include <ctime>

#include <chrono>
#include <thread>

#include "input.h"

#include <cassert>

#define SCREEN_WIDTH 83 // Default width of windows terminal (wsl) when
                        // taking up half of screen
#define SCREEN_HEIGHT 40 // Default height is 42. So setting this height 40
                         // leaves us two extra rows to print fps and other
                         // info

#define MAP_WIDTH 20  // Number of columns in map (width)
#define MAP_HEIGHT 20 // Number of rows in map (height)
#define MAP_TILES (MAP_WIDTH * MAP_HEIGHT) // Number of tiles in map

#define PI 3.14159
#define FOV (PI / 4) // Field of view angle

float playerX = 6.0f; // Player x position/coordinate
float playerY = 6.0f; // Player y position/coordinate
float playerA = 0.0f; // Player angle of direction its looking at

// Raycast Distance Resolution, float number. 
// Represents how fine the resolution is when calculating the distance of a ray.
// - If its equal to 1.0f, that means the resolution is one tile, meaning the finest level
//   we can differ distance between two ray casts is one map tile.
// - The smaller distance this is, the better distance rendering will look, but it will 
//   simultaneously bring down performance slightly.
#define RAYCAST_DIST_RES 0.05

// Max field depth. Maximum distance player can see, value has 1:1 ratio to map tile
#define MAX_DEPTH 20

int main()
{
    std::string map;
    // # = wall/obastacle
    // . = space
    map += "####################";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "####################";

    printf("\033c"); // Clear screen

    // Matrix where we store the characters in how they will be rendered onto the screen
    std::string screen(SCREEN_WIDTH * SCREEN_HEIGHT, ' ');

    init_input();

    clock_t prevClock = clock();

    // Game loop
    while (1)
    {
        // Handle input
        if (kbhit())
        {
            char key = getch();
            if (key == 'k') // rotate ccw
            {
                playerA -= 0.04f;
            }
            else if (key == 'l') // rotate cw
            {
                playerA += 0.04f;
            }
            else if (key == 'w') // move forwards
            {
                playerX += sinf(playerA);
                playerY += cosf(playerA);

                // Collision detection
                if (map[(int)playerY * MAP_WIDTH + (int)playerX] == '#')
                {
                    // New position puts us inside a wall. Rollback the move we just did
                    playerX -= sinf(playerA);
                    playerY -= cosf(playerA);
                }
            }
            else if (key == 's') // move backwards
            {
                playerX -= sinf(playerA);
                playerY -= cosf(playerA);

                // Collision detection
                if (map[(int)playerY * MAP_WIDTH + (int)playerX] == '#')
                {
                    // New position puts us inside a wall. Rollback the move we just did
                    playerX += sinf(playerA);
                    playerY += cosf(playerA);
                }
            }
            else if (key == 'a') // strafe left
            {
                playerX += sinf(playerA - 1.57); // pi/2 (90 degrees) = 1.57
                playerY += cosf(playerA - 1.57);

                // Collision detection
                if (map[(int)playerY * MAP_WIDTH + (int)playerX] == '#')
                {
                    // New position puts us inside a wall. Rollback the move we just did
                    playerX -= sinf(playerA - 1.57);
                    playerY -= cosf(playerA - 1.57);
                }
            }
            else if (key == 'd') // strafe right
            {
                playerX += sinf(playerA + 1.57);
                playerY += cosf(playerA + 1.57);

                // Collision detection
                if (map[(int)playerY * MAP_WIDTH + (int)playerX] == '#')
                {
                    // New position puts us inside a wall. Rollback the move we just did
                    playerX -= sinf(playerA + 1.57);
                    playerY -= cosf(playerA + 1.57);
                }
            }
        }

        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            // For each column, making up the screen, calculate the projected ray angle into world space
            // ---- CALCULATION EXPLAINED: ----
            // (playerA - FOV / 2.0f) = The left edge of our field-of-view (what we see)
            // (float)x / (float)SCREEN_WIDTH) * FOV = If x = 1 this is how much degree of angle for each column we see infront of us,
            //                                   So this adds the amount of degrees of angle to find our column
            float rayAngle = (playerA - FOV / 2.0f) + ((float)x / (float)SCREEN_WIDTH) * FOV;

            // Progressively step forward in direction of current 
            // 'rayAngle' until we hit a wall to figure out the distance.
            float distanceToWall = 0;
            bool hitWall = false;

            // Unit vector for ray
            float eyeX = sinf(rayAngle);
            float eyeY = cosf(rayAngle);
            while (!hitWall && distanceToWall < MAX_DEPTH)
            {
                distanceToWall += RAYCAST_DIST_RES;

                int testX = (int)(playerX + eyeX * distanceToWall);
                int testY = (int)(playerY + eyeY * distanceToWall);
            
                // Test if ray is out of bounds
                if ( testX < 0 ||
                     testX >= MAP_WIDTH ||
                     testY < 0 ||
                     testY >= MAP_HEIGHT)
                {
                    hitWall = true;
                    distanceToWall = MAX_DEPTH;
                }
                else
                {
                    // Ray is inbounds so test to see if the ray cell is a wall block
                    if (map[testY * MAP_WIDTH + testX] == '#')
                    {
                        hitWall = true;
                    }
                }
            }

            // Calculate how much of ceiling and floor should show based on the distance 
            // (more ceiling and floor the further away wall is)

            // We assume out eyes are at Horizon level, so the further away we are
            // we can think the height of the wall as it appears shrinks closer and closer
            // to the middle as we move further away, so it shrinks in how it appears equally
            // from the floor as it does from the ceiling.
            int ceiling = (float)(SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / ((float) distanceToWall);
            int floor = SCREEN_HEIGHT - ceiling;

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

        // Sacrifice the right most column for newlines
        for (int i = 0; i < SCREEN_HEIGHT; ++i)
        {
            int last_pixel = i * (SCREEN_WIDTH) + (SCREEN_WIDTH-1); // Get last pixel/char of row
            screen[last_pixel] = '\n';
        }
        screen[SCREEN_WIDTH*SCREEN_HEIGHT - 1] = '\0';

        move(0,0);
        printw("%s\n", screen.c_str());

        static unsigned long frameCounter = 0;
        clock_t clock_diff = clock() - prevClock;
        printw("\nclock_diff = %ld, clocksPerSec = %ld\n", clock_diff, CLOCKS_PER_SEC);
        double time_diff_sec = (double)clock_diff / CLOCKS_PER_SEC;
        long fps = 1.0f / time_diff_sec;
        printw("FPS = %ld TimeDiff: %f seconds, frameCounter = %lu", fps, time_diff_sec, frameCounter);
        prevClock = clock();
        frameCounter++;
        refresh(); // Without this printw will not be outputted before

        //std::chrono::milliseconds timespan(1000); // or whatever
        //std::this_thread::sleep_for(timespan);
    } // End of Game loop ( while(1) )
}
