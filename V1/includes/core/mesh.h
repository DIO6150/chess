#ifndef H_CHESS_MESH
#define H_CHESS_MESH

#include "core/texture.h"
#include "core/vertices.h"

#include "vendor/cglm/cglm.h"
#include "vendor/cglm/mat4.h"

typedef struct
{
    int n_vertices;
    int n_indices;

    Vertices3D *vertices;
    int *indices;

    Texture* tex;

    mat4 model_matrix;

    int visible;
} Mesh;

Mesh mcg_CreateMesh(Vertices3D *vertices, int vertices_count, int *indices, int indices_count);
void mcg_FreeMesh(Mesh *mesh);

void mcg_AddTexture(Mesh *mesh, Texture* tex);

#endif