#include "game/graphics/piece2D.h"

Mesh mcg_CreatePiece2D(__attribute__((unused))Type type, __attribute__((unused))Color color)
{
    Vertices3D vertices[4] = {
        { 0, 2, 0,  1, 1, 1,  0, 1,  0, 0, 0 },
        { 2, 2, 0,  1, 1, 1,  1, 1,  0, 0, 0 },
        { 2, 0, 0,  1, 1, 1,  1, 0,  0, 0, 0 },
        { 0, 0, 0,  1, 1, 1,  0, 0,  0, 0, 0 }
    };

    int indices[6] = {
        0, 1, 2, 2, 3, 0
    };

    return mcg_CreateMesh(vertices, 4, indices, 6);
}