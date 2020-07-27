#ifndef POINT_H
#define POINT_H

/*Point is a struct for the coordinates of a certain cell*/
struct point
{
    int y; /*row coordinate*/
    int x; /*col*/
};

/*constructor for Point*/
Point new_point(int x, int y);
void free_point_cells(Point **matrix, int n);

#endif