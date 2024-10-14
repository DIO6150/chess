#include "glad/glad.h"

#include "core/renderer.h"


/*   BATCH   */

int mgeBatchInit(int _in_batch_id, Batch** _out_batch)
{
    (*_out_batch) = (Batch*) malloc(sizeof(Batch));

    if (!*_out_batch)
    {
        printf("%s(%s) : Failed to init batch.\n", __func__, __FILE__);
        return (0);
    }

    (*_out_batch)->batch_id = _in_batch_id;

    (*_out_batch)->persistent = 0;
    (*_out_batch)->shader = NULL;

    (*_out_batch)->mesh_max_count = 0;
    (*_out_batch)->vertex_max_count = 0;
    (*_out_batch)->index_max_count = 0;

    (*_out_batch)->mesh_count = 0;
    (*_out_batch)->vertex_count = 0;
    (*_out_batch)->index_count = 0;

    glGenVertexArrays(1, &(*_out_batch)->vao);

    glGenBuffers(1, &(*_out_batch)->vbo);
    glGenBuffers(1, &(*_out_batch)->ebo);
    glGenBuffers(1, &(*_out_batch)->dibo);

    glGenBuffers(1, &(*_out_batch)->matrix_ssbo);

    // since at this point, _out_batch is initialized, we don't do error handling
    mgeBatchBindBuffers(*_out_batch);

    glNamedBufferData((*_out_batch)->vbo, 1, NULL, GL_DYNAMIC_DRAW);
    glNamedBufferData((*_out_batch)->ebo, 1, NULL, GL_DYNAMIC_DRAW);
    glNamedBufferData((*_out_batch)->dibo, 1, NULL, GL_DYNAMIC_DRAW);
    glNamedBufferData((*_out_batch)->matrix_ssbo, 1, NULL, GL_DYNAMIC_DRAW);

    // TODO : Allow custom vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    mgeBatchUnbindBuffers();

    return (1);
}

int mgeBatchFree(Batch* _in_batch)
{
    if (!_in_batch)
    {
        printf("%s(%s) : Batch was not initialized, aborting.\n", __func__, __FILE__);
        return (0);
    }

    glDeleteVertexArrays(1, &_in_batch->vao);

    glDeleteBuffers(1, &_in_batch->vbo);
    glDeleteBuffers(1, &_in_batch->ebo);
    glDeleteBuffers(1, &_in_batch->dibo);

    glDeleteBuffers(1, &_in_batch->matrix_ssbo);

    free(_in_batch);

    return (1);
}

int mgeBatchBindBuffers(Batch* _in_batch)
{
    if (!_in_batch)
    {
        printf("%s(%s) : Batch was not initialized.\n", __func__, __FILE__);
        return (0);
    }

    glBindVertexArray(_in_batch->vao);

    glBindBuffer(GL_ARRAY_BUFFER, _in_batch->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _in_batch->ebo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _in_batch->dibo);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _in_batch->matrix_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _in_batch->matrix_ssbo);

    return (1);
}

int mgeBatchUnbindBuffers()
{
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    return (1);
}

int mgeBatchResize(Batch* _in_batch, int _in_mesh_count, int _in_vertex_count, int _in_index_count)
{
    if (!_in_batch)
    {
        printf("%s(%s) : Batch was not initialized\n", __func__, __FILE__);
        return (0);
    }

    void* old_vbo_data = NULL;
    void* old_ebo_data = NULL;
    void* old_dibo_data = NULL;
    void* old_matrix_ssbo_data = NULL;

    // TODO : There is actually undefined behavior here,
    // If I get old data with old size into an old size container then fuck me I guess ?
    // Because the data packed next to it is not mine and therefore i'm shit

    if (_in_batch->mesh_count != 0)
    {

        old_vbo_data = glMapNamedBuffer(_in_batch->vbo, GL_READ_WRITE);
        old_ebo_data = glMapNamedBuffer(_in_batch->ebo, GL_READ_WRITE);
        old_dibo_data = glMapNamedBuffer(_in_batch->dibo, GL_READ_WRITE);
        old_matrix_ssbo_data = glMapNamedBuffer(_in_batch->matrix_ssbo, GL_READ_WRITE);

        if (!old_vbo_data || !old_ebo_data || !old_dibo_data || !old_matrix_ssbo_data)
        {
            if (old_vbo_data)
                glUnmapNamedBuffer(_in_batch->vbo);

            if (old_ebo_data)
                glUnmapNamedBuffer(_in_batch->ebo);

            if (old_dibo_data)
                glUnmapNamedBuffer(_in_batch->dibo);

            if (old_matrix_ssbo_data)
                glUnmapNamedBuffer(_in_batch->matrix_ssbo);

            printf("%s(%s) : Error when mapping OpenGL buffer.\n", __func__, __FILE__);
            return (0);
        }     
    }

    _in_batch->mesh_max_count += _in_mesh_count;
    _in_batch->vertex_max_count += _in_vertex_count;
    _in_batch->index_max_count += _in_index_count;

    glNamedBufferData(_in_batch->vbo, _in_batch->vertex_max_count * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
    glNamedBufferSubData(_in_batch->vbo, 0, _in_batch->vertex_count * sizeof(Vertex), old_vbo_data);

    glNamedBufferData(_in_batch->ebo, _in_batch->index_max_count * sizeof(int), NULL, GL_DYNAMIC_DRAW);
    glNamedBufferSubData(_in_batch->ebo, 0, _in_batch->index_count * sizeof(int), old_ebo_data);

    glNamedBufferData(_in_batch->dibo, _in_batch->mesh_max_count * sizeof(DrawCommand), NULL, GL_DYNAMIC_DRAW);
    glNamedBufferSubData(_in_batch->dibo, 0, _in_batch->mesh_count * sizeof(DrawCommand), old_dibo_data);

    glNamedBufferData(_in_batch->matrix_ssbo, _in_batch->mesh_max_count * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);
    glNamedBufferSubData(_in_batch->matrix_ssbo, 0, _in_batch->mesh_count * sizeof(mat4), old_matrix_ssbo_data);

    return (1);
}

int mgeBatchPushMesh(Batch* _in_batch, Mesh* _in_mesh)
{
    if (!_in_batch)
    {
        printf("%s(%s) : Batch is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_mesh)
    {
        printf("%s(%s) : Mesh is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (_in_batch->vertex_count + _in_mesh->vertex_count >= _in_batch->vertex_max_count || 
        _in_batch->index_count + _in_mesh->index_count >= _in_batch->index_max_count)

    {
        if (!mgeBatchResize(_in_batch, 1, _in_mesh->vertex_count, _in_mesh->index_count))
        {
            printf("%s(%s) : Couldn't resize, aborting.\n", __func__, __FILE__);
            printf("Mesh Count : %d / %d\n", _in_batch->mesh_count, _in_batch->mesh_max_count);
            return (0);
        }
    }

    // TODO : Take into account instance rendering
    DrawCommand command;

    command.count = _in_mesh->index_count;
    command.instance_count = 1;
    command.first_index = _in_batch->index_count;
    command.base_vertex = _in_batch->vertex_count;
    command.base_instance = 0;

    // Vertex Data
    glNamedBufferSubData(_in_batch->vbo, _in_batch->vertex_count * sizeof(Vertex), _in_mesh->vertex_count * sizeof(Vertex), _in_mesh->vertices);

    // Index Data
    glNamedBufferSubData(_in_batch->ebo, _in_batch->index_count * sizeof(int), _in_mesh->index_count * sizeof(int), _in_mesh->indices);

    // Draw Command Data
    glNamedBufferSubData(_in_batch->dibo, _in_batch->mesh_count * sizeof(DrawCommand), sizeof(DrawCommand), &command);

    // Matrix Data

    mat4 model_matrix;

    glm_mat4_mul(_in_mesh->rotation_matrix, _in_mesh->scaling_matrix, model_matrix);
    glm_mat4_mul(model_matrix, _in_mesh->translation_matrix, model_matrix);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _in_batch->matrix_ssbo);
    glNamedBufferSubData(_in_batch->matrix_ssbo, _in_batch->mesh_count * sizeof(mat4), sizeof(mat4), model_matrix);

    // Texture2D Data
    // nothing to upload

    _in_batch->mesh_count++;
    _in_batch->vertex_count += _in_mesh->vertex_count;
    _in_batch->index_count += _in_mesh->index_count;

    return (1);
}

int mgeBatchReset(Batch* _in_batch)
{
    if (!_in_batch)
    {
        printf("%s(%s) : Batch was not initialized.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_batch->persistent)
    {
        _in_batch->mesh_count = 0;
        _in_batch->vertex_count = 0;
        _in_batch->index_count = 0;
    }

    return (1);
}

/*   RENDERER   */

int mgeRendererInit(Renderer** _out_renderer)
{
    *_out_renderer = malloc(sizeof(Renderer));

    if (!(*_out_renderer))
    {
        printf("%s(%s) : Renderer couldn't be initialized.\n", __func__, __FILE__);
        return (0);
    }

    for (int i = 0; i < MGE_BATCH_MAX_COUNT; i++)
        (*_out_renderer)->batches[i] = NULL;

    (*_out_renderer)->batch_count = 0;

    (*_out_renderer)->textures = NULL;
    (*_out_renderer)->textures_capacity = 0;
    (*_out_renderer)->texture_count = 0;

    (*_out_renderer)->atlas = malloc(sizeof(Texture2D));

    if (!(*_out_renderer)->atlas)
    {
        printf("%s(%s) : Atlas couldn't be initialized.\n", __func__, __FILE__);
        (*_out_renderer)->atlas = 0;
    }

    // rounded value of 45° in radians
    glm_perspective(0.785398f, 1.0f, 0.1f, 100.0f, (*_out_renderer)->projection);

    if (!mgeCameraInit(&((*_out_renderer)->camera), (vec3) { 0.0f, 0.0f, 3.0f }))
    {
        if ((*_out_renderer)->atlas) mgeTextureObjectFree((*_out_renderer)->atlas);
        free(*_out_renderer);
        printf("%s(%s) : Camera failed to initialized, aborting.\n", __func__, __FILE__);
        return (0);
    }

    return (1);
}

int mgeRendererFree(Renderer* _in_renderer)
{
    if (!_in_renderer)
    {
        printf("%s(%s) : Renderer isn't initialized.\n", __func__, __FILE__);
        return (0);
    }

    for (int i = 0; i < _in_renderer->batch_count; i++)
    {
        mgeBatchFree(_in_renderer->batches[i]);
    }

    mgeTextureObjectFree(_in_renderer->atlas);
    mgeCameraFree(_in_renderer->camera);

    free(_in_renderer);

    return (1);
}

int mgeRendererAddTexture(Renderer* _in_renderer, Texture2D* _in_texture)
{
    if (!_in_renderer)
    {
        printf("%s(%s) : Renderer is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_renderer->textures)
    {
        _in_renderer->textures = malloc(sizeof(Texture2D*));

        if (!_in_renderer->textures)
        {
            printf("%s(%s) : Failed to allocate textures list memory.\n", __func__, __FILE__);
            return (0);
        }

        _in_renderer->textures_capacity = 1;
    }

    if (_in_renderer->texture_count >= _in_renderer->textures_capacity)
    {
        Texture2D** temp = realloc(_in_renderer->textures, (_in_renderer->textures_capacity + 1) * sizeof(Texture2D*));

        if (!temp)
        {
            printf("%s(%s) : Failed to reallocate textures list memory.\n", __func__, __FILE__);
            return (0);
        }

        _in_renderer->textures = temp;
        _in_renderer->textures_capacity++;
    }

    _in_renderer->textures[_in_renderer->texture_count] = _in_texture;
    _in_renderer->texture_count++;

    return (1);
}

int mgeRendererCreateAtlas(Renderer* _in_renderer)
{
    if (!_in_renderer->atlas)
    {
        printf("%s(%s) : Atlas is NULL.\n", __func__, __FILE__);
        return (0);
    }



    return (1);
}

int mgeRendererSetStatic(Renderer* _in_renderer, int _in_batch_id, int _in_persist)
{
    if (!_in_renderer)
    {
        printf("%s(%s) : Renderer is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (_in_batch_id < 0 || _in_batch_id >= MGE_BATCH_MAX_COUNT)
    {
        printf("%s(%s) : Invalid index : %d.\n", __func__, __FILE__, _in_batch_id);
        return (0);
    }

    if (!_in_renderer->batches[_in_batch_id])
    {
        if (!mgeBatchInit(_in_batch_id, _in_renderer->batches +_in_batch_id))
        {
            printf("%s(%s) : Couldn't create a new batch, aborting.\n", __func__, __FILE__);
            return (0);
        }

        _in_renderer->batch_count++;
    }

    _in_renderer->batches[_in_batch_id]->persistent = _in_persist;

    return (1);
}

int mgeRendererSetBatchShader(Renderer* _in_renderer, int _in_batch_id, Shader* _in_shader)
{
    if (!_in_renderer)
    {
        printf("%s(%s) : Renderer is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_shader)
    {
        printf("%s(%s) : Shader is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (_in_batch_id < 0 || _in_batch_id >= MGE_BATCH_MAX_COUNT)
    {
        printf("%s(%s) : Invalid index : %d.\n", __func__, __FILE__, _in_batch_id);
        return (0);
    }

    if (!_in_renderer->batches[_in_batch_id])
    {
        if (!mgeBatchInit(_in_batch_id, _in_renderer->batches +_in_batch_id))
        {
            printf("%s(%s) : Couldn't create a new batch, aborting.\n", __func__, __FILE__);
            return (0);
        }

        _in_renderer->batch_count++;
    }

    _in_renderer->batches[_in_batch_id]->shader = _in_shader;

    return (1);
}

int mgeRendererPushMesh(Renderer* _in_renderer, int _in_batch_id, Mesh* _in_mesh)
{
    if (!_in_renderer)
    {
        printf("%s(%s) : Renderer is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_mesh)
    {
        printf("%s(%s) : Mesh is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (_in_batch_id < 0 || _in_batch_id >= MGE_BATCH_MAX_COUNT)
    {
        printf("%s(%s) : Invalid index : %d.\n", __func__, __FILE__, _in_batch_id);
        return (0);
    }

    if (_in_batch_id >= _in_renderer->batch_count)
    {
        if (!_in_renderer->batches[_in_batch_id])
        {
            if (!mgeBatchInit(_in_batch_id, _in_renderer->batches +_in_batch_id))
            {
                printf("%s(%s) : Couldn't create a new batch, aborting.\n", __func__, __FILE__);
                return (0);
            }

            _in_renderer->batch_count++;
        }
    }

    Batch* batch = _in_renderer->batches[_in_batch_id];

    // even if there is an error, I can't really do anything about it so i'm not catching it.
    mgeBatchPushMesh(batch, _in_mesh);

    return (1);
}

int mgeRender(Renderer* _in_renderer)
{
    if (!_in_renderer)
    {
        printf("%s(%s) : Renderer isn't initialized.\n", __func__, __FILE__);
        return (0);
    }

    mat4 view;

    if (!mgeCameraGetView(_in_renderer->camera, view))
    {
        printf("%s(%s) : Couldn't create camera view, aborting.\n", __func__, __FILE__);
        return (0);
    }

    for (int i = 0; i < _in_renderer->batch_count; i++)
    {
        if (!_in_renderer->batches[i])
        {
            printf("%s(%s) : Batch [%d] is NULL.\n", __func__, __FILE__, i);
            continue;
        }

        if (!mgeBatchBindBuffers(_in_renderer->batches[i]))
        {
            printf("%s(%s) : Error when binding buffers of batch [%d]\n", __func__, __FILE__, i);
            continue;
        }

        if (!_in_renderer->batches[i]->shader)
        {
            printf("%s(%s) : Shader of batch [%d] is NULL.\n", __func__, __FILE__, i);
            continue;
        }

        mgeShaderUse(_in_renderer->batches[i]->shader);
        int location;

        location = glGetUniformLocation(_in_renderer->batches[i]->shader->program, "uProjection");
        glUniformMatrix4fv(location, 1, GL_FALSE, *_in_renderer->projection);

        location = glGetUniformLocation(_in_renderer->batches[i]->shader->program, "uView");
        glUniformMatrix4fv(location, 1, GL_FALSE, *view);

        /*
        // for debugging
        Vertex* __attribute__((unused)) vbo_ptr = (Vertex*) glMapNamedBuffer(_in_renderer->batches[i]->vbo, GL_READ_ONLY);
        unsigned int* __attribute__((unused)) ebo_ptr = (unsigned int*) glMapNamedBuffer(_in_renderer->batches[i]->ebo, GL_READ_ONLY);
        DrawCommand* __attribute__((unused)) dibo_ptr = (DrawCommand*) glMapNamedBuffer(_in_renderer->batches[i]->dibo, GL_READ_ONLY);
        
        glUnmapNamedBuffer(_in_renderer->batches[i]->vbo);
        glUnmapNamedBuffer(_in_renderer->batches[i]->ebo);
        glUnmapNamedBuffer(_in_renderer->batches[i]->dibo);
        */
       
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, _in_renderer->batches[i]->mesh_count, 0);

        mgeShaderCancel();

        mgeBatchUnbindBuffers();

        mgeBatchReset(_in_renderer->batches[i]);
    }

    return (1);
}