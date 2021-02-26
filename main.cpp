#include <string> // stoi, string
#include <cmath>
#include <ctime>

#include <chrono>
#include <thread>

#include "globals.h"
#include "input.h"
#include "rendering.h"

#include <cassert>
#include <algorithm> // max
#include <iostream> // cin
#include <vector>
#include <unistd.h> // isatty

float playerX = 2.0f; // Player x position/coordinate
float playerY = 8.0f; // Player y position/coordinate
float playerA = 0.0f; // Player angle of direction its looking at

// Definition of extern variables from "globals.h"
int screen_width;
int screen_height;

int main(int argc,char* argv[])
{
    printf("\033c"); // Clear screen

    // Try parsing command line arguments screen height and width
    try
    {
        if (argc >= 3)
        {
            screen_height = (std::stoi(argv[1]) - 3); // Minus 3 to make space for the prinout of fps, player position etc.
            screen_width = std::stoi(argv[2]);
        }
        else
        {
            throw std::out_of_range("Out of range on argv");
        }
    }
    catch(const std::exception& e)
    {
        printf("Could not find and parse command line args for screen height and width.\nGoing with default values.\n");
        screen_height = DEFAULT_SCREEN_HEIGHT;
        screen_width = DEFAULT_SCREEN_WIDTH;
    }
    printf("Screen Width = %d Height = %d\n", screen_width, screen_height);
    printf("Used WASD to move forward/backward and strafe left/right. Use K and L to rotate.\n");
    printf("Press Enter to continue...\n");
    sleep(1);
    std::cin.ignore();

    std::string map;
    // # = wall/obastacle
    // . = space
    map += "####################";
    map += "#..................#";
    map += "#..................#";
    map += "####...............#";
    map += "#..................#";
    map += "#............#.....#";
    map += "#............#######";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..........####....#";
    map += "#..........####....#";
    map += "#..........####....#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "#..................#";
    map += "####################";

    printf("\033c"); // Clear screen

    // Matrix where we store the characters in how they will be rendered onto the screen
    std::string screen(screen_width * screen_height, ' ');

    // True = Colorized rendering/output
    // False = Pure ascii (white text on black background) rendering/output
    bool colored_output = true;

    init_input();
    init_colors();

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
                playerA -= 0.08f;
            }
            else if (key == 'l') // rotate cw
            {
                playerA += 0.08f;
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
            else if (key == 'v') // Switch visual mode (toggle between ascii and colorized drawing)
            {
                colored_output = !colored_output;
                attrset(A_NORMAL); // Override previous attributes set, and set to normal
                                   // if switching is to ascii rendering
                                   // (attron doesn,'t override previous, attrset does though)
            }
        }

        // Should only be called once per frame (as suppose to once per column or row)
        // Needs also to be called before rendering wall, as wall should paint over
        // the ceiling and floor.
        if (colored_output)
        {
            colored_draw_ceiling_and_floor();
        }

        // Iterate through all screen columns
        for (int x = 0; x < screen_width; ++x)
        {
            // For each column, making up the screen, calculate the projected ray angle into world space
            // ---- CALCULATION EXPLAINED: ----
            // (playerA - FOV / 2.0f) = The left edge of our field-of-view (what we see)
            // (float)x / (float)screen_width) * FOV = If x = 1 this is how much degree of angle for each column we see infront of us,
            //                                   So this adds the amount of degrees of angle to find our column
            float rayAngle = (playerA - FOV / 2.0f) + ((float)x / (float)screen_width) * FOV;

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
            int ceiling = std::max( (float)(screen_height / 2.0) - screen_height / ((float) distanceToWall), 0.0f );
            int floor = screen_height - ceiling;

            if (colored_output)
            {
                colored_draw_wall_column(x, ceiling, floor, distanceToWall);
            }
            else
            {
                ascii_shade_column(x, ceiling, floor, distanceToWall, screen);
            }
        }

        if (colored_output)
        {
            // So the next printout, fps printout etc., happens on the correct line
            move(screen_height, 0);
            // Make sure next printout doesn't have any of the background or foreground color applied
            attrset(A_NORMAL);
        }
        else
        {
            ascii_draw(screen);
        }

        static unsigned long frameCounter = 0;
        clock_t clock_diff = clock() - prevClock;
        printw("\nclock_diff = %ld, clocksPerSec = %ld\n", clock_diff, CLOCKS_PER_SEC);
        double time_diff_sec = (double)clock_diff / CLOCKS_PER_SEC;
        long fps = 1.0f / time_diff_sec;
        printw("FPS = %ld TimeDiff: %f seconds, frameCounter = %lu\n", fps, time_diff_sec, frameCounter);
        prevClock = clock();
        frameCounter++;
        printw("player pos (x,y) = %.3f,%.3f playerA = %.3f", playerX, playerY, playerA);
        refresh(); // Without this printw will not be outputted before

    } // End of Game loop ( while(1) )
}
