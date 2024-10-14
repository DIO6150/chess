#ifndef H_MGE_MESH
#define H_MGE_MESH

#include "core/vertex.h"
#include "core/texture.h"

#include "cglm/cglm.h"
#include "cglm/mat4.h"

typedef struct
{
    unsigned int index;

    Vertex* vertices;
    int vertex_count;

    unsigned int* indices;
    int index_count;

    mat4 translation_matrix;
    mat4 rotation_matrix;
    mat4 scaling_matrix;

    Texture2D* texture;
} Mesh;

int mgeMeshInit(unsigned int _in_index, Vertex* _in_vertices, unsigned int* _in_indices, int _in_vertex_count, int _in_index_count, Mesh** _out_mesh, Texture2D* texture);
int mgeMeshFree(Mesh* _in_mesh);

int mgeMeshTranslate(Mesh* _in_mesh, vec3 _in_translation);
int mgeMeshRotate(Mesh* _in_mesh, float theta, vec3 _in_axis);
int mgeMeshScale(Mesh* _in_mesh, vec3 _in_scale);

#endif