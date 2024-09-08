#ifndef H_CHESS_MESH
#define H_CHESS_MESH

#include "core/vertices.h"

typedef enum
{
    CHESS_MESH_TYPE_2D = 0,
    CHESS_MESH_TYPE_3D = 1
} MeshType;

typedef struct
{
    MeshType type;
    int n_vertices;
    int n_indices;

    int *indices;
    union
    {
        Vertices2D *v2D;
        Vertices3D *v3D;
    } vertices;

} Mesh;

Mesh mcg_CreateMesh2D(Vertices2D *vertices, int vertices_count, int *indices, int indices_count);
void mcg_FreeMesh(Mesh *mesh);

#endif