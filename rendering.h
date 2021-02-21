#include <string> // std::string

void ascii_shade_column(int x, int ceiling, int floor, float distanceToWall, std::string &screen);
void ascii_draw(std::string &screen);

void init_colors();

// Draws/Renders one column of the wall with each call
void colored_draw_wall_column(int x, int ceiling, int floor, float distanceToWall);

// Draws/Renders the colored ceiling and floor
// ( Needs only to be called once per frame
//   not once per column drawn.
//   Also important that its called before wall is rendered
//   as Wall is meant to be rendered ontop of what is rendered
//   by this function. )
void colored_draw_ceiling_and_floor();
