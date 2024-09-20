#include "core/mesh.h"

#include <stdlib.h>

#include "vendor/cglm/cglm.h"
#include "vendor/cglm/mat4.h"

Mesh mcg_CreateMesh(Vertices3D *vertices, int vertices_count, int *indices, int indices_count)
{
    Mesh mesh;
    mesh.vertices = malloc(sizeof(Vertices3D) * vertices_count);

    if (mesh.vertices == NULL)
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
        mesh.vertices[i] = vertices[i];
    }

    for (int i = 0; i < indices_count; i++)
    {
        mesh.indices[i] = indices[i];
    }

    mesh.visible = 1; 

    return mesh;
}

void mcg_FreeMesh(Mesh *mesh)
{
    free(mesh->vertices);
    free(mesh->indices);
}

void mcg_AddTexture(Mesh *mesh, Texture* tex)
{
    mesh->tex = tex;
}