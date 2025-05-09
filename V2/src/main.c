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

    vec3 colors[7] = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
    };

    Vertex base_vertices[] =
    {
        { 0.0f, 0.5f, 0.0f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.5f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f },

        { 0.0f, 0.5f,-0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.5f,-0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f,-0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f },

        { 0.0f, 0.5f,-0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.5f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f },

        { 0.5f, 0.5f,-0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.5f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f },

        { 0.5f, 0.5f,-0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.5f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.5f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.5f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f },

        { 0.5f, 0.0f,-0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.5f, 0.0f, 0.0f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f,-0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f },
    };

    unsigned int base_indices[] =
    {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20,
    };

    int width =  10;
    int height = 10;
    int depth =  10;
    int n_mesh = width * height * depth;
    
    Mesh** meshes;
    meshes = (Mesh**) malloc(n_mesh * sizeof(Mesh*));

    for (int i = 0; i < n_mesh; i++)
    {

        for (int j = 0; j < 4 * 6; j++)
        {
            vec3 *color = colors + ((j / 4) % 7);
            base_vertices[j].r = (*color)[0];
            base_vertices[j].g = (*color)[1];
            base_vertices[j].b = (*color)[2];
        }


        mgeMeshInit(0, base_vertices, base_indices, 4 * 6, 6 * 6, meshes + i, NULL);
        mgeMeshTranslate(meshes[i], (vec3) { 1.0f * (i % width), 1.0f * ((i % (width * height)) / height), 1.0f * (i / (width * height)) });
    }

    Shader* shader;
    char* vertex_source;
    char* fragment_source;

    vertex_source =
    "#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec2 aTexCoords;\n"
    "layout (location = 2) in vec3 aColor;\n"
    "layout(std430, binding = 0) buffer u_ModelMatrices\n"
    "{\n"
    "   mat4 matrices[];\n"
    "};\n"
    "out vec3 oColor;\n"
    "uniform mat4 uProjection;\n"
    "uniform mat4 uView;\n"
    "void main()\n"
    "{\n"
    "   vec4 pos = uProjection * uView * matrices[gl_DrawID] * vec4(aPos, 1.0);\n"
    "   gl_Position = pos;\n"
    "   oColor = aColor;\n"
    //"   oColor = vec3((uProjection * uView *  matrices[gl_DrawID] * vec4(aPos, 1.0)).w, 0.0, 0.0);\n"
    "}\0";

    fragment_source =
    "#version 460 core\n"
    "in vec3 oColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(oColor, 1.0);\n"
    "}\0";

    if (!mgeShaderCreate(vertex_source, fragment_source, &shader))
    {
        free(meshes);

        mgeRendererFree(renderer);

        mgeContextFree(context);

        return (EXIT_FAILURE);
    }

    mgeRendererSetStatic(renderer, 0, 1);
    mgeRendererSetStatic(renderer, 1, 1);
    mgeRendererSetStatic(renderer, 2, 1);
    mgeRendererSetStatic(renderer, 3, 1);
    mgeRendererSetStatic(renderer, 4, 1);
    mgeRendererSetStatic(renderer, 5, 1);
    mgeRendererSetStatic(renderer, 6, 1);

    mgeRendererSetBatchShader(renderer, 0, shader);
    mgeRendererSetBatchShader(renderer, 1, shader);
    mgeRendererSetBatchShader(renderer, 2, shader);
    mgeRendererSetBatchShader(renderer, 3, shader);
    mgeRendererSetBatchShader(renderer, 4, shader);
    mgeRendererSetBatchShader(renderer, 5, shader);
    mgeRendererSetBatchShader(renderer, 6, shader);

    for (int i = 0; i < n_mesh; i++)
    {
        mgeRendererPushMesh(renderer, 1, meshes[i]);
    }

    mgeContextShouldClose(context, &context_should_close);

    // Time

    double fps_limit  = 1.0 / 120.0;
    double last_update = 0.0;
    double last_frame  = 0.0;

    double now = 0.0;
    __attribute__((unused)) double delta_time = 0.0;

    // Useful values

    double pos[2];
    double last_pos[2];

    glfwGetCursorPos(context->window, pos, pos + 1);
    glfwGetCursorPos(context->window, last_pos, last_pos + 1);

    const float camera_speed = 10.0f;

    while(!context_should_close)
    {
        /* EVENTS BIT */

        glfwPollEvents();

        now = glfwGetTime();
        delta_time = now - last_update;

        glfwGetCursorPos(context->window, pos, pos + 1);

        // printf("Pos %f %f | Last Pos %f %f\n", pos[0], pos[1], last_pos[0], last_pos[1]);

        /* UPDATE BIT */

        if (glfwGetKey(context->window, GLFW_KEY_W) == GLFW_PRESS)
            mgeCameraMoveFromFront(renderer->camera, (vec3) { camera_speed * delta_time, camera_speed * delta_time, camera_speed * delta_time });

        if (glfwGetKey(context->window, GLFW_KEY_S) == GLFW_PRESS)
            mgeCameraMoveFromFront(renderer->camera, (vec3) { camera_speed *-delta_time, camera_speed *-delta_time, camera_speed *-delta_time });


        if (glfwGetKey(context->window, GLFW_KEY_A) == GLFW_PRESS)
            mgeCameraMoveFromRight(renderer->camera, (vec3) { camera_speed *-delta_time, camera_speed *-delta_time, camera_speed *-delta_time });

        if (glfwGetKey(context->window, GLFW_KEY_D) == GLFW_PRESS)
            mgeCameraMoveFromRight(renderer->camera, (vec3) { camera_speed * delta_time, camera_speed * delta_time, camera_speed * delta_time });


        if (glfwGetKey(context->window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            
        }

        //float xoffset = pos[0] - last_pos[0];
        //float yoffset = last_pos[1] - pos[1];
        last_pos[0] = pos[0];
        last_pos[1] = pos[1];

        /*float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;*/

        if (glfwGetKey(context->window, GLFW_KEY_LEFT) == GLFW_PRESS)
            mgeCameraLook(renderer->camera, (float) delta_time * -camera_speed * 8.0f, 0.0f);

        if (glfwGetKey(context->window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            mgeCameraLook(renderer->camera, (float) delta_time *  camera_speed * 8.0f, 0.0f);

        if (glfwGetKey(context->window, GLFW_KEY_UP) == GLFW_PRESS)
            mgeCameraLook(renderer->camera, 0.0f, (float) delta_time * camera_speed * 8.0f);

        if (glfwGetKey(context->window, GLFW_KEY_DOWN) == GLFW_PRESS)
            mgeCameraLook(renderer->camera, 0.0f, (float) delta_time *-camera_speed * 8.0f);

        /* RENDER BIT */

        if (now - last_frame >= fps_limit)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mgeRender(renderer);

            glfwSwapBuffers(context->window);

            last_frame = now;
        }

        mgeContextShouldClose(context, &context_should_close);

        last_update = now;
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