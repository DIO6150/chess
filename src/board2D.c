#include <stdlib.h>

#include "game/graphics/board2D.h"


Mesh mcg_CreateBoard2D()
{
    Vertices2D vertices[4] = {
        {-0.25,  0.25, 1, 0, 0, 0, 1},
        { 0.25,  0.25, 0, 1, 0, 1, 1},
        { 0.25, -0.25, 0, 0, 1, 1, 0},
        {-0.25, -0.25, 1, 0, 1, 0, 0}
    };

    int indices[6] = {
        0, 1, 2, 2, 1, 3
    };

    return mcg_CreateMesh2D(vertices, 4, indices, 6);;
}

void mcg_FillBoard2D(__attribute__((unused))Piece** board)
{

}