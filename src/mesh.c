#include "core/mesh.h"

#include <stdlib.h>

#include "vendor/cglm/cglm.h"
#include "vendor/cglm/mat4.h"

Mesh mcg_CreateMesh2D(Vertices2D *vertices, int vertices_count, int *indices, int indices_count)
{
    Mesh mesh;
    mesh.type = CHESS_MESH_TYPE_2D;
    mesh.vertices.v2D = malloc(sizeof(Vertices2D) * vertices_count);

    if (mesh.vertices.v2D == NULL)
    {
        exit(EXIT_FAILURE);
    }

    mesh.indices = malloc(sizeof(int) * indices_count);

    if (mesh.indices == NULL)
    {
        exit(EXIT_FAILURE);
    }

    mesh.n_vertices = vertices_count;
    mesh.n_indices = indices_count;

    mesh.tex = NULL;

    glm_mat4_identity(mesh.model_matrix);

    for (int i = 0; i < vertices_count; i++)
    {
        mesh.vertices.v2D[i] = vertices[i];
    }

    for (int i = 0; i < indices_count; i++)
    {
        mesh.indices[i] = indices[i];
    }

    return mesh;
}

void mcg_FreeMesh(Mesh *mesh)
{
    // possibly we can free only one of the union member but i don't want to confuse the compiler and I can't find an answer on stackoverflow so yeeee
    if (mesh->type == CHESS_MESH_TYPE_2D)
    {
        free(mesh->vertices.v2D);
    }
    else
    {
        free(mesh->vertices.v3D);
    }
    free(mesh->indices);
}

void mcg_AddTexture(Mesh *mesh, Texture* tex)
{
    mesh->tex = tex;
}