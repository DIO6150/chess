#include <stdio.h>
#include <stdlib.h>

#include "core/renderer.h"

#include "vendor/glad/glad.h"
#include "vendor/GLFW/glfw3.h"

void mcg_InitRenderPool(RenderPool* pool, int material, MeshType type)
{
    pool->type = type;
    pool->material = material;
    pool->size = 0;

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

    if (type == CHESS_MESH_TYPE_2D)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2D) * MAX_VERTEX_PER_MESH * BATCH_SIZE, NULL, GL_DYNAMIC_DRAW);
    }
    else if (type == CHESS_MESH_TYPE_3D)
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices3D) * MAX_VERTEX_PER_MESH * BATCH_SIZE, NULL, GL_DYNAMIC_DRAW);
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * MAX_VERTEX_PER_MESH * BATCH_SIZE, NULL, GL_DYNAMIC_DRAW);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, BATCH_SIZE * sizeof(DrawCommand), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pool->matrix_ssbo);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, BATCH_SIZE * sizeof(mat4), (const void*)0, GL_DYNAMIC_STORAGE_BIT);


    // TODO : have a dedicated layout system later
    int layouts[2][4] = {
        { 2, 3, 2 },
        { 3, 3, 2, 3 }
    };

    int layout_size[2] = { 3, 4 };

    int stride;
    int offset;

    offset = 0;
    
    for (int i = 0; i < layout_size[type]; i++)
    {
        stride += layouts[type][i];
    }

    for (int i = 0; i < layout_size[type]; i++)
    {
        glVertexAttribPointer(i, layouts[type][i], GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);

        offset += layouts[type][i];
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

    // TODO : Difference between 3d and 2d shaders

    const char *vertexShaderSource =
    "#version 460 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoords;\n"
    "layout(std430, binding = 0) buffer u_ModelMatrices\n"
    "{\n"
    "   mat4 matrices[];\n"
    "};\n"
    "out vec3 oColor;\n"
    "out vec2 oTexCoords;\n"
    "void main()\n"
    "{\n"
    "   vec4 pos = vec4(aPos.x, aPos.y, 0.0, 1.0) * matrices[gl_DrawID];\n"
    "   pos.z = 0.0;\n"
    "   pos.w = 1.0;\n"
    "   gl_Position = pos;\n"
    "   oColor = aColor;\n"
    "   oTexCoords = aTexCoords;\n"
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
    "in vec2 oTexCoords;\n"
    "in vec3 oColor;\n"
    "uniform sampler2D texture_0;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   vec4 tex_color = texture(texture_0, oTexCoords);\n"
    "   FragColor = tex_color * vec4(oColor, 1.0f);\n"
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

    return temp;
}

void mcg_FreeRenderer(Renderer* renderer)
{
    free(renderer);
}

static void fill_data(Mesh* mesh, int vbo, int vbo_offset, int ebo, int ebo_offset)
{
    if (mesh->type == CHESS_MESH_TYPE_2D)
    {
        glNamedBufferSubData(vbo, sizeof(Vertices2D) * vbo_offset, sizeof(Vertices2D) * mesh->n_vertices, mesh->vertices.v2D);
    }
    else if (mesh->type == CHESS_MESH_TYPE_3D)
    {
        glNamedBufferSubData(vbo, sizeof(Vertices3D) * vbo_offset, sizeof(Vertices3D) * mesh->n_vertices, mesh->vertices.v3D);
    }

    glNamedBufferSubData(ebo, sizeof(int) * ebo_offset, sizeof(int) * mesh->n_indices, mesh->indices);
}

void mcg_PushModel(Renderer* renderer, Mesh* object, int material)
{
    for (int i = 0; i < renderer->count; i++)
    {

        if (renderer->batches[i].type != object->type)
            continue;

        if (renderer->batches[i].size >= BATCH_SIZE)
            continue;

        if (renderer->batches[i].material == material)
        {
            renderer->batches[i].objects[renderer->batches[i].size] = object;
            renderer->batches[i].size++;

            fill_data(object,
            renderer->batches[i].vbo,
            renderer->batches[i].vbo_offset,
            renderer->batches[i].ebo,
            renderer->batches[i].ebo_offset);

            renderer->batches[i].vbo_offset += object->n_vertices;
            renderer->batches[i].ebo_offset += object->n_indices;

            return;
        }
    }

    if (renderer->count >= MAX_BATCH)
        return;

    mcg_InitRenderPool(renderer->batches + renderer->count, material, object->type);

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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        int location = glGetUniformLocation(renderer->shader, "texture_0");

        if (location < 0)
        {
            printf("Uniform was not found.");
        }

        glUniform1ui(location, textures[1]);

        //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, renderer->batches[i].textures_ssbo);
		//glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, count * sizeof(int), textures);

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