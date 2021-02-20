#include <string>
#include <cmath>
#include <ctime>

#include <chrono>
#include <thread>

#include "constants.h"
#include "input.h"
#include "rendering.h"

#include <cassert>

float playerX = 6.0f; // Player x position/coordinate
float playerY = 6.0f; // Player y position/coordinate
float playerA = 0.0f; // Player angle of direction its looking at

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

            // We assume our eyes are at Horizon level, so the further away we are
            // we can think the height of the wall as it appears shrinks closer and closer
            // to the middle as we move further away, so it shrinks in how it appears equally
            // from the floor as it does from the ceiling.
            int ceiling = (float)(SCREEN_HEIGHT / 2.0) - SCREEN_HEIGHT / ((float) distanceToWall);
            int floor = SCREEN_HEIGHT - ceiling;

            ascii_shade_column(x, ceiling, floor, distanceToWall, screen);
        }

        ascii_draw(screen);

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
