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

    pool->vbo_offset = 0;
    pool->ebo_offset = 0;


    glGenVertexArrays(1, &pool->vao);
    glBindVertexArray(pool->vao);

    glGenBuffers(1, &pool->vbo);
    glGenBuffers(1, &pool->ebo);
    glGenBuffers(1, &pool->dibo);
    //printf("dibo : %d\n", pool->dibo);

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

RenderPool* mcg_CreateRenderPool()
{
    return malloc(sizeof(RenderPool));
}

void mcg_FreeRenderPool(RenderPool* pool)
{
    free(pool);
}

Renderer* mcg_CreateRenderer()
{
    Renderer* temp = malloc(sizeof(Renderer));
    temp->count = 0;

    const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    temp->shader = glCreateProgram();

    glAttachShader(temp->shader, vertexShader);
    glAttachShader(temp->shader, fragmentShader);
    glLinkProgram( temp->shader);

    return temp;
}

void mcg_FreeRenderer(Renderer* renderer)
{
    free(renderer);
}

static void fill_data(Mesh* mesh, int vbo, int vbo_offset, int ebo, int ebo_offset)
{
    //printf("fill data\n");

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
    //printf("create draw commands\n");
    //printf("DrawCommand Begin\n");
    DrawCommand* temp;

    temp = malloc(sizeof(DrawCommand) * pool->size);

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

        index += vertex_count;

        base_vertex += pool->objects[i]->n_vertices;

        //printf("Count : %d\n", *count);
        (*count)++;
    }
    //printf("DrawCommand End\n");

    return temp;
}

void mcg_Render(Renderer* renderer)
{
    DrawCommand *cmd;
    int count;

    //printf("Render Begin\n");

    //printf("Render\n");
    for (int i = 0; i < renderer->count; i++)
    {

        //printf("> Render Batch [%d]\n", i);

        cmd = mcg_CreateDrawCommands(&renderer->batches[i], &count);
        //printf("    > Batch Count [%d]\n", count);
        
        glUseProgram(renderer->shader);

        glBindVertexArray(renderer->batches[i].vao);

        glBindBuffer(GL_ARRAY_BUFFER, renderer->batches[i].vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->batches[i].ebo);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderer->batches[i].dibo);

        glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, count * sizeof(DrawCommand), cmd);
        /*printf("Bind draw indirect\n");

        __attribute__((unused))int size_dc = sizeof(DrawCommand);
        __attribute__((unused))int size_cmd = sizeof(*cmd);

        printf("draw : dibo : %d ; count : %d ; i : %d ; batches : %d\n", renderer->batches[i].dibo, count, i, renderer->count);

        GLint boundBuffer;

        fflush(stdout);

        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &boundBuffer);
        printf("Currently bound ARRAY buffer: %d\n", boundBuffer);

        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundBuffer);
        printf("Currently bound ELEMENT buffer: %d\n", boundBuffer);

        glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &boundBuffer);
        printf("Currently bound DRAW INDIRECT buffer: %d\n", boundBuffer);

        fflush(stdout);

        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_STRIDE, &boundBuffer);
        printf("Currently max vertex attrib stride: %d\n", boundBuffer);

        printf("Size of int %lld and of glint %lld\n", sizeof(int), sizeof(GLint));

        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &boundBuffer);
        printf("Buffer size: %d\n", boundBuffer);
        */

        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, count, 0);

        glBindVertexArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

        glUseProgram(0);

        renderer->batches[i].size = 0;
        renderer->batches[i].vbo_offset = 0;
        renderer->batches[i].ebo_offset = 0;

        //printf("further\n");
        free(cmd);
    }
    //printf("Render End\n");
}