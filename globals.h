// globals.h - Holds Constants and variables to be available globally in the whole code base.

#ifndef GLOBALS_H
#define GLOBALS_H

// If screen width and height is not provided
// to program by command line arguments, these
// are the values screen width and height is set to.
#define DEFAULT_SCREEN_WIDTH 83 // Default width of windows terminal (wsl) when
                                // taking up half of screen
#define DEFAULT_SCREEN_HEIGHT 40 // Default height is 42. So setting this height 40
                                 // leaves us two extra rows to print fps and other
                                 // info

#define MAP_WIDTH 20  // Number of columns in map (width)
#define MAP_HEIGHT 20 // Number of rows in map (height)
#define MAP_TILES (MAP_WIDTH * MAP_HEIGHT) // Number of tiles in map

#define PI 3.14159
#define FOV (PI / 4) // Field of view angle

// Raycast Distance Resolution, float number. 
// Represents how fine the resolution is when calculating the distance of a ray.
// - If its equal to 1.0f, that means the resolution is one tile, meaning the finest level
//   we can differ distance between two ray casts is one map tile.
// - The smaller distance this is, the better distance rendering will look, but it will 
//   simultaneously bring down performance slightly.
#define RAYCAST_DIST_RES 0.01

// Max field depth. Maximum distance player can see, value has 1:1 ratio to map tile
#define MAX_DEPTH 15

// The official screen width and height.
// The values represents number of characters in width and height.
extern int screen_width;
extern int screen_height;

#endif
