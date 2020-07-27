#include "typedefs.h"
#include "stdlib.h"
#include "point.h"
/*x col coordinate, y row coordinate*/
Point new_point(int x, int y)
{
    Point point_res;
    point_res.x = x;
    point_res.y = y;
    return point_res;
}

void free_point_cells(Point **matrix, int n)
{
    int i;
    for(i = 0; i < n; ++i)
    {
        free(matrix[i]);
    }
    free(matrix);
}