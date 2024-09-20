#ifndef H_MGE_MESH
#define H_MGE_MESH

#include "core/vertex.h"

#include "cglm/cglm.h"
#include "cglm/mat4.h"

typedef struct
{
    Vertex* vertices;
    int vertex_count;

    unsigned int* indices;
    int index_count;

    mat4 model_matrix;
} Mesh;

int mgeMeshInit(Vertex* _in_vertices, unsigned int* _in_indices, int _in_vertex_count, int _in_index_count, Mesh** _out_mesh);
int mgeMeshFree(Mesh* _in_mesh);

#endif