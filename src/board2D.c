#include <stdlib.h>

#include "game/graphics/board2D.h"

#include "vendor/cglm/cglm.h"
#include "vendor/cglm/mat4.h"
#include "vendor/cglm/affine.h"

Mesh mcg_CreateBoard2D()
{
    Vertices2D base_vertices[4] = {
        { 0, 2, 1, 0, 0, 0, 1 },
        { 2, 2, 0, 1, 0, 1, 1 },
        { 2, 0, 0, 0, 1, 1, 0 },
        { 0, 0, 1, 0, 1, 0, 0 }
    };

    int base_indices[6] = {
        0, 1, 2, 2, 3, 0
    };

    Vertices2D vertices[4 * 64];
    int indices[6 * 64];

    int vert_index;
    int inde_index;

    vert_index = 0;
    inde_index = 0;

    float scale = 1.0;

    for (int i = 0; i < 64; i++)
    {
        vert_index = i * 4;
        inde_index = i * 6;

        for (int j = 0; j < 4; j++)
        {
            vertices[vert_index + j] = base_vertices[j];

            vertices[vert_index + j].x *= scale;
            vertices[vert_index + j].y *= scale;

            vertices[vert_index + j].x += (i % 8) * scale * 2;
            vertices[vert_index + j].y += (i / 8) * scale * 2;

            vertices[vert_index + j].r = 1.0f;
            vertices[vert_index + j].g = 1.0f;
            vertices[vert_index + j].b = 1.0f;

            if ( ( (i % 8) + (i / 8) ) % 2 == 0)
            {
                vertices[vert_index + j].r = 0.0f;
                vertices[vert_index + j].g = 0.0f;
                vertices[vert_index + j].b = 0.0f;
            }

        }

        for (int j = 0; j < 6; j++)
        {
            indices[inde_index + j] = base_indices[j] + vert_index;
        }
    }

    Mesh return_mesh;

    return_mesh = mcg_CreateMesh2D(vertices, 64 * 4, indices, 64 * 6);

    return return_mesh;
}

void mcg_FillBoard2D(__attribute__((unused))Piece** board)
{

}