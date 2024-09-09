#include "game/graphics/piece2D.h"

Mesh mcg_CreatePiece2D(__attribute__((unused))Type type, Color color)
{
    Vertices2D vertices[4] = {
        { 0, 2, 1, 0, 0, 0, 1 },
        { 2, 2, 0, 1, 0, 1, 1 },
        { 2, 0, 0, 0, 1, 1, 0 },
        { 0, 0, 1, 0, 1, 0, 0 }
    };

    int indices[6] = {
        0, 1, 2, 2, 3, 0
    };

    for (int i = 0; i < 4; i++)
    {
        if (color == CHESS_COLOR_BLACK)
        {
            vertices[i].r = 0.1;
            vertices[i].g = 0.1;
            vertices[i].b = 0.1;
        }
        else if (color == CHESS_COLOR_WHITE)
        {
            vertices[i].r = 0.9;
            vertices[i].g = 0.9;
            vertices[i].b = 0.9;
        }
    }

    return mcg_CreateMesh2D(vertices, 4, indices, 6);
}