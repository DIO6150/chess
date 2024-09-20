
#include "core/mesh.h"

int mgeMeshInit(Vertex* _in_vertices, unsigned int* _in_indices, int _in_vertex_count, int _in_index_count, Mesh** _out_mesh)
{
    *_out_mesh = (Mesh*) malloc(sizeof(Mesh));

    (*_out_mesh)->vertices = _in_vertices;
    (*_out_mesh)->indices = _in_indices;

    /*(*_out_mesh)->vertices = (Vertex*) malloc(sizeof(Vertex) * _in_vertex_count);

    if (!(*_out_mesh)->vertices)
    {
        printf("%s(%s) : Couldn't initialize vertex memory.\n", __func__, __FILE__);
        return (0);
    }

    (*_out_mesh)->indices = (int*) malloc(sizeof(int) * _in_index_count);

    if (!(*_out_mesh)->indices)
    {
        free((*_out_mesh)->vertices);
        free((*_out_mesh));
        printf("%s(%s) : Couldn't initialize index memory.\n", __func__, __FILE__);
        return (0);
    }

    for (int i = 0; i < _in_vertex_count; i++)
        (*_out_mesh)->vertices[i] = _in_vertices[i];

    for (int i = 0; i < _in_index_count; i++)
        (*_out_mesh)->indices[i] = _in_indices[i];*/

    (*_out_mesh)->vertex_count = _in_vertex_count;
    (*_out_mesh)->index_count = _in_index_count;

    glm_mat4_identity((*_out_mesh)->model_matrix);

    return (1);
}

int mgeMeshFree(Mesh* _in_mesh)
{
    if (!_in_mesh)
    {
        printf("%s(%s) : Mesh is NULL.\n", __func__, __FILE__);
        return (0);
    }

    //free(_in_mesh->vertices);
    //free(_in_mesh->indices);
    free(_in_mesh);

    return (1);
}