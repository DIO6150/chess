#include <stdio.h>
#include <stdlib.h>

#include "core/renderer.h"

#include "vendor/glad/glad.h"
#include "vendor/GLFW/glfw3.h"

void mcg_InitRenderPool(RenderPool* pool, int id)
{
    pool->pool_id = id;
    pool->max_size = 0;
    pool->size = 0;

    pool->objects = (Mesh**)malloc(1);

    pool->n_verticies = 0;
    pool->n_indicies = 0;

    pool->vao = 0;
    pool->vbo = 0;
    pool->ebo = 0;
    pool->dibo = 0;
    pool->matrix_ssbo = 0;
    pool->textures_ssbo = 0;

    pool->vbo_offset = 0;
    pool->ebo_offset = 0;


    glGenVertexArrays(1, &pool->vao);
    glBindVertexArray(pool->vao);

    glGenBuffers(1, &pool->vbo);
    glGenBuffers(1, &pool->ebo);
    glGenBuffers(1, &pool->dibo);
    glGenBuffers(1, &pool->matrix_ssbo);

    glBindBuffer(GL_ARRAY_BUFFER, pool->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pool->ebo);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, pool->dibo);

    glBufferData(GL_ARRAY_BUFFER, 1, NULL, GL_STREAM_DRAW);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 1, NULL, GL_STREAM_DRAW);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, 1, NULL, GL_STREAM_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pool->matrix_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1, (const void*)0, GL_STREAM_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pool->matrix_ssbo);

    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, pool->texture_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 1, (const void*)0, GL_STREAM_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pool->matrix_ssbo);


    // TODO : have a dedicated layout system later
    int layout[4] = { 3, 3, 2, 3 };

    int stride = 3 + 3 + 2 + 3;
    int offset = 0;

    for (int i = 0; i < 4; i++)
    {
        glVertexAttribPointer(i, layout[i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);

        offset += layout[i];
    }

    glBindVertexArray(0);
}

void mcg_FreeRenderPool(RenderPool* pool)
{
    free(pool);
}

Renderer* mcg_CreateRenderer()
{
    Renderer* temp = malloc(sizeof(Renderer));

    if (temp == NULL)
    {
        exit(EXIT_FAILURE);
    }


    temp->count = 0;

    // TODO : Implements multi shader system

    const char *vertexShaderSource =
    "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoords;\n"
    "layout(std430, binding = 0) buffer u_ModelMatrices\n"
    "{\n"
    "   mat4 matrices[];\n"
    "};\n"
    "flat out int oDrawID;\n"
    "out vec3 oColor;\n"
    "out vec2 oTexCoords;\n"
    "void main()\n"
    "{\n"
    "   vec4 pos = vec4(aPos, 1.0) * matrices[gl_DrawID];\n"
    "   pos.w = 1.0;\n"
    "   gl_Position = pos;\n"
    "   oColor = aColor;\n"
    "   oTexCoords = aTexCoords;\n"
    "   oDrawID = gl_DrawID;\n"
    "}\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    const char *fragmentShaderSource =
    "#version 460 core\n"
    "uniform bool has_texture;\n"
    "uniform sampler2D texture_0;\n"
    "layout(std430, binding = 1) buffer u_Textures\n"
    "{\n"
    "   int tex[];\n"
    "};\n"
    "flat in int oDrawID;\n"
    "in vec3 oColor;\n"
    "in vec2 oTexCoords;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   if (has_texture)\n"
    "   {\n"
    "       vec4 tex_color = texture(texture_0, oTexCoords);\n"
    "       FragColor = tex_color * vec4(oColor, 1.0f);\n"
    "   }\n"
    "   else\n"
    "   {\n"
    "       FragColor = vec4(oColor, 1.0f);\n"
    "   }\n"
    "}\0";

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }

    temp->shader = glCreateProgram();

    glAttachShader(temp->shader, vertexShader);
    glAttachShader(temp->shader, fragmentShader);
    glLinkProgram(temp->shader);

    glGetProgramiv(temp->shader, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(temp->shader, 512, NULL, infoLog);
        printf("%s\n", infoLog);
    }


    temp->batches = malloc(sizeof(RenderPool));

    if (temp->batches == NULL)
    {
        printf("(mcg_CreateRenderer) Error when allocating batch memory");
    }

    return temp;
}

void mcg_FreeRenderer(Renderer* renderer)
{
    free(renderer->batches);
    free(renderer);
}

static void fill_data(Mesh* mesh, int vbo, int vbo_offset, int ebo, int ebo_offset)
{
    glNamedBufferSubData(vbo, sizeof(Vertices3D) * vbo_offset, sizeof(Vertices3D) * mesh->n_vertices, mesh->vertices);
    glNamedBufferSubData(ebo, sizeof(int) * ebo_offset, sizeof(int) * mesh->n_indices, mesh->indices);
}

void mcg_ResizePool(RenderPool* pool, int size_offset, int max_vertex_offset, int max_index_offset)
{
    void *old_vbo = NULL;
    void *old_ebo = NULL;
    void *old_dibo = NULL;
    void *old_matrix_ssbo = NULL;

    if (pool->size != 0)
    {
        old_vbo = glMapNamedBufferRange(pool->vbo, 0, pool->n_verticies * sizeof(Vertices3D), GL_MAP_READ_BIT);
        old_ebo = glMapNamedBufferRange(pool->ebo, 0, pool->n_indicies * sizeof(int), GL_MAP_READ_BIT);
        old_dibo = glMapNamedBufferRange(pool->dibo, 0, pool->size * sizeof(DrawCommand), GL_MAP_READ_BIT);
        old_matrix_ssbo = glMapNamedBufferRange(pool->matrix_ssbo, 0, pool->size * sizeof(mat4), GL_MAP_READ_BIT);

        if (old_vbo == NULL || old_ebo == NULL || old_dibo == NULL || old_matrix_ssbo == NULL)
        {
            printf("(mcg_ResizePool) Error when maping opengl buffer data.\n");
            exit(EXIT_FAILURE);
        }
    }

    pool->max_size += size_offset;
    pool->n_verticies += max_vertex_offset;
    pool->n_indicies += max_index_offset;

    glNamedBufferData(pool->vbo, pool->n_verticies * sizeof(Vertices3D), old_vbo, GL_STREAM_DRAW);
    glNamedBufferData(pool->ebo, pool->n_indicies * sizeof(int), old_ebo, GL_STREAM_DRAW);
    glNamedBufferData(pool->dibo, pool->max_size * sizeof(DrawCommand), old_dibo, GL_STREAM_DRAW);
	glNamedBufferData(pool->matrix_ssbo, pool->max_size * sizeof(mat4), old_matrix_ssbo, GL_STREAM_DRAW);

    pool->objects = realloc(pool->objects, pool->max_size * sizeof(Mesh*));

    if (pool->objects == NULL)
    {
        printf("(mcg_ResizePool) Error when reallocating pool memory.\n");
        exit(EXIT_FAILURE);
    }

}

void mcg_PushModel(Renderer* renderer, Mesh* object, PoolID id)
{
    for (int i = 0; i < renderer->count; i++)
    {
        if (renderer->batches[i].pool_id == id)
        {
            if (renderer->batches[i].size >= renderer->batches[i].max_size)
            {
                mcg_ResizePool(renderer->batches + i, 1, object->n_vertices, object->n_indices);
            }

            if (renderer->batches[i].objects[renderer->batches[i].size] != object)
            {
                renderer->batches[i].objects[renderer->batches[i].size] = object;

                fill_data(object,
                renderer->batches[i].vbo,
                renderer->batches[i].vbo_offset,
                renderer->batches[i].ebo,
                renderer->batches[i].ebo_offset);
            }

            renderer->batches[i].size++;
            renderer->batches[i].vbo_offset += object->n_vertices;
            renderer->batches[i].ebo_offset += object->n_indices;

            return;
        }
    }

    RenderPool *new_pools = realloc(renderer->batches, (renderer->count + 1) * sizeof(RenderPool));

    if (new_pools == NULL)
    {
        mcg_FreeRenderer(renderer);

        printf("(mcg_PushModel) Error when trying to reallocate pool array.\n");
        exit(EXIT_FAILURE);
    }

    renderer->batches = new_pools;

    printf("(mcg_PushModel) Info : Created Pool\n");
    mcg_InitRenderPool(renderer->batches + renderer->count, renderer->count);
    mcg_ResizePool(renderer->batches + renderer->count, 1, object->n_vertices, object->n_indices);

    renderer->batches[renderer->count].objects[0] = object;
    renderer->batches[renderer->count].size++;

    fill_data(object,
    renderer->batches[renderer->count].vbo,
    renderer->batches[renderer->count].vbo_offset,
    renderer->batches[renderer->count].ebo,
    renderer->batches[renderer->count].ebo_offset);

    renderer->batches[renderer->count].vbo_offset += object->n_vertices;
    renderer->batches[renderer->count].ebo_offset += object->n_indices;

    renderer->count++;

    return;
}

DrawCommand* mcg_CreateDrawCommands(RenderPool* pool, int* count)
{
    DrawCommand* temp;

    temp = malloc(sizeof(DrawCommand) * pool->size);

    if (temp == NULL)
    {
        exit(EXIT_FAILURE);
    }

    *count = 0;

    int vertex_count;
    int base_vertex;
    int index;

    base_vertex = 0;
    index = 0;

    // TODO : doesn't into account instancing
    for (int i = 0; i < pool->size; i++)
    {
        vertex_count = pool->objects[i]->n_indices;

        temp[i].count = vertex_count;
        temp[i].instanceCount = 1;
        temp[i].firstIndex = index;
        temp[i].baseVertex = base_vertex;
        temp[i].baseInstance = 0;

        index += pool->objects[i]->n_indices;

        base_vertex += pool->objects[i]->n_vertices;

        (*count)++;
    }

    return temp;
}

void mcg_Render(Renderer* renderer)
{
    DrawCommand *cmd;
    int count;

    float *model_matrices;

    unsigned int *textures;

    for (int i = 0; i < renderer->count; i++)
    {

        cmd = mcg_CreateDrawCommands(&renderer->batches[i], &count);

        model_matrices = malloc(sizeof(float) * 16 * count);

        if (model_matrices == NULL)
        {
            exit(EXIT_FAILURE);
        }

        textures = malloc(sizeof(unsigned int) * count);

        if (textures == NULL)
        {
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < count; j++)
        {

            if (renderer->batches[i].objects[j]->tex)
            {
                textures[j] = renderer->batches[i].objects[j]->tex->object;
            }
            else
            {
                textures[j] = 0;
            }

            for (int k = 0; k < 16; k++)
            {
                model_matrices[k + j * 16] = renderer->batches[i].objects[j]->model_matrix[k % 4][k / 4];
            }
        }
        
        glUseProgram(renderer->shader);

        //glGetUniformLocation(renderer->shader, "model");
        //glUniformMatrix4fv(renderer->shader, 1, 0, renderer->batches[i].);

        glBindVertexArray(renderer->batches[i].vao);

        glBindBuffer(GL_ARRAY_BUFFER, renderer->batches[i].vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->batches[i].ebo);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderer->batches[i].dibo);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderer->batches[i].matrix_ssbo);
	    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, count * sizeof(mat4), model_matrices);

        glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, count * sizeof(DrawCommand), cmd);
        
        int location;

        // Should be a SSBO but I guess it's fine for now
        location = glGetUniformLocation(renderer->shader, "has_texture");

        if (location < 0)
        {
            printf("Uniform was not found.\n");
        }

        glUniform1i(location, textures[i]);

        if (textures[i])
        {
            // TODO : one day, a pool will support multiple textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textures[0]);

            location = glGetUniformLocation(renderer->shader, "texture_0");

            if (location < 0)
            {
                printf("Uniform was not found.\n");
            }

            glUniform1i(location, 0);
        }
        //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, renderer->batches[i].textures_ssbo);
		//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, count * sizeof(int), textures);

        //#include "vertices.h"

        Vertices3D* __attribute__((unused)) vbo_ptr  =  (Vertices3D*) glMapNamedBuffer(renderer->batches[i].vbo, GL_READ_ONLY);

        glUnmapNamedBuffer(renderer->batches[i].vbo);

        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, count, 0);

        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

        glUseProgram(0);

        renderer->batches[i].size = 0;
        renderer->batches[i].vbo_offset = 0;
        renderer->batches[i].ebo_offset = 0;

        free(model_matrices);
        free(textures);
        free(cmd);
    }
}
