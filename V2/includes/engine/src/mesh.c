
#include <stdlib.h>
#include <string.h>

#include "core/mesh.h"


int mgeMeshInit(Vertex* _in_vertices, unsigned int* _in_indices, int _in_vertex_count, int _in_index_count, Mesh** _out_mesh, Texture* texture)
{
    *_out_mesh = (Mesh*) malloc(sizeof(Mesh));

    //(*_out_mesh)->vertices = _in_vertices;
    //(*_out_mesh)->indices = _in_indices;

    (*_out_mesh)->vertices = (Vertex*) malloc(sizeof(Vertex) * _in_vertex_count);

    if (!(*_out_mesh)->vertices)
    {
        printf("%s(%s) : Couldn't initialize vertex memory.\n", __func__, __FILE__);
        return (0);
    }

    (*_out_mesh)->indices = (unsigned int*) malloc(sizeof(int) * _in_index_count);

    if (!(*_out_mesh)->indices)
    {
        free((*_out_mesh)->vertices);
        free((*_out_mesh));
        printf("%s(%s) : Couldn't initialize index memory.\n", __func__, __FILE__);
        return (0);
    }

    memcpy((*_out_mesh)->vertices, _in_vertices, _in_vertex_count * sizeof(Vertex));
    memcpy((*_out_mesh)->indices, _in_indices, _in_index_count * sizeof(int));

    (*_out_mesh)->vertex_count = _in_vertex_count;
    (*_out_mesh)->index_count = _in_index_count;

    glm_translate_make((*_out_mesh)->translation_matrix, (vec3) {0.0f, 0.0f, 0.0f});
    glm_rotate_make((*_out_mesh)->rotation_matrix, 0.0f, (vec3) {0.0f, 0.0f, 0.0f});
    glm_scale_make((*_out_mesh)->scaling_matrix, (vec3) {1.0f, 1.0f, 1.0f});

    (*_out_mesh)->texture = texture;
    
    return (1);
}

int mgeMeshFree(Mesh* _in_mesh)
{
    if (!_in_mesh)
    {
        printf("%s(%s) : Mesh is NULL.\n", __func__, __FILE__);
        return (0);
    }

    free(_in_mesh->vertices);
    free(_in_mesh->indices);
    free(_in_mesh);

    return (1);
}

int mgeMeshTranslate(Mesh* _in_mesh, vec3 _in_translation)
{
    if (!_in_mesh)
    {
        printf("%s(%s) : Mesh is NULL.\n", __func__, __FILE__);
        return (0);
    }

    glm_translate(_in_mesh->translation_matrix, _in_translation);

    return (1);
}

int mgeMeshRotate(Mesh* _in_mesh, float theta, vec3 _in_axis)
{
    if (!_in_mesh)
    {
        printf("%s(%s) : Mesh is NULL.\n", __func__, __FILE__);
        return (0);
    }

    glm_rotate(_in_mesh->rotation_matrix, theta, _in_axis);

    return (1);
}

int mgeMeshScale(Mesh* _in_mesh, vec3 _in_scale)
{
    if (!_in_mesh)
    {
        printf("%s(%s) : Mesh is NULL.\n", __func__, __FILE__);
        return (0);
    }

    glm_scale(_in_mesh->scaling_matrix, _in_scale);

    return (1);
}