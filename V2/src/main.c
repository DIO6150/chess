#include <stdio.h>
#include <stdlib.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "core/context.h"
#include "core/renderer.h"

int main (__attribute__((unused)) int argc, __attribute__((unused)) char** argv)
{

    // Context Creation

    Context* context = NULL;
    Renderer* renderer = NULL;
    int context_should_close = 0;

    if(!mgeContextInit("Hello World", 800, 800, &context))
    {
        return EXIT_FAILURE;
    }

    if (!mgeRendererInit(&renderer))
    {
        return EXIT_FAILURE;
    }

    // Meshes

    Vertex base_vertices[4] =
    {
        { 0.0f, 0.5f, 0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.5f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f },
        { 0.5f, 0.0f, 0.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 1.0f }
    };

    unsigned int base_indices[6] =
    {
        0, 1, 2, 2, 3, 0
    };

    int n_mesh = 1;
    
    Mesh** meshes;

    meshes = (Mesh**) malloc(n_mesh * sizeof(Mesh*));

    for (int i = 0; i < n_mesh; i++)
        mgeMeshInit(base_vertices, base_indices, 4, 6, meshes + i);

    Shader* shader;
    char* vertex_source;
    char* fragment_source;

    vertex_source =
    "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 2) in vec3 aColor;\n"
    "layout(std430, binding = 0) buffer u_ModelMatrices\n"
    "{\n"
    "   mat4 matrices[];\n"
    "};\n"
    "out vec3 oColor;\n"
    "void main()\n"
    "{\n"
    "   vec4 pos = vec4(aPos, 1.0) * matrices[gl_DrawID];\n"
    "   pos.w = 1.0;\n"
    "   gl_Position = vec4(aPos, 1.0f);\n"
    "   oColor = aColor;\n"
    "}\0";

    fragment_source =
    "#version 460 core\n"
    "in vec3 oColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(oColor, 1.0f);\n"
    "}\0";

    if (!mgeShaderCreate(vertex_source, fragment_source, &shader))
    {
        free(meshes);

        mgeRendererFree(renderer);

        mgeContextFree(context);

        return (EXIT_FAILURE);
    }

    mgeRendererSetBatchShader(renderer, 0, shader);

    mgeContextShouldClose(context, &context_should_close);

    while(!context_should_close)
    {
        glfwSwapBuffers(context->window);

        for (int i = 0; i < n_mesh; i++)
            mgeRendererPushMesh(renderer, 0, meshes[i]);

        mgeRender(renderer);

        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

        mgeContextShouldClose(context, &context_should_close);
    }

    for (int i = 0; i < n_mesh; i++)
    {
        mgeMeshFree(meshes[i]);
    }

    free(meshes);

    mgeRendererFree(renderer);

    mgeContextFree(context);

    return EXIT_SUCCESS;
};