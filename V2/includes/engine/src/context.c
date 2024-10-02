#include <stdlib.h>
#include <stdio.h>

#include "core/context.h"

static void message_callback(__attribute__((unused))GLenum source, GLenum type, GLuint id, GLenum severity, __attribute__((unused))GLsizei length, const GLchar    * message, __attribute__((unused))const void* userParam)
{
    printf("Severity : %d, Type : %d, ID : %d, Message : %s\n", severity, type, id, message);
}

void framebuffer_size_callback(__attribute__((unused))GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int mgeContextInit(char* _in_name, int _in_width, int _in_height, Context** _out_context)
{
    *_out_context = malloc(sizeof(Context));

    if (*_out_context == NULL)
    {
        printf("%s(%s) : Couldn't allocate memory.\n", __func__, __FILE__);
        return (0);
    }

    if (!glfwInit())
    {
        free(*_out_context);
        printf("%s(%s) : GLFW failed to initialize.\n", __func__, __FILE__);
        return (0);
    }
  
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "falling.table.mgengine.0.0");

    (*_out_context)->window = glfwCreateWindow(_in_width, _in_height, _in_name, NULL, NULL);

    if (!(*_out_context)->window)
    {
        free(*_out_context);
        glfwTerminate();
        printf("%s(%s) : Failed to create window.\n", __func__, __FILE__);
        return (0); 
    }

    // glfwSetWindowAttrib((*_out_context)->window, GLFW_RESIZABLE, GLFW_FALSE);

    glfwMakeContextCurrent((*_out_context)->window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwDestroyWindow((*_out_context)->window);
        free(*_out_context);
        glfwTerminate();
        printf("%s(%s) : Failed to initialize glad.\n", __func__, __FILE__);
        return(0);
    }
    printf("GL %d.%d\n", GLVersion.major, GLVersion.minor);
    
    glViewport(0, 0, _in_width, _in_height);
    
    glfwSetFramebufferSizeCallback((*_out_context)->window, framebuffer_size_callback);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
    //glCullFace(GL_FRONT);
    //glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, 0);
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, NULL, GL_TRUE);

    return (1);
}

int mgeContextFree(Context* _in_context)
{
    if (!_in_context)
    {
        printf("%s(%s) : Context is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_context->window)
    {
        printf("%s(%s) : Context window is NULL.\n", __func__, __FILE__);
        return (0);
    }

    glfwDestroyWindow(_in_context->window);

    free(_in_context);

    glfwTerminate();


    return (1);
}

int mgeContextFocused(Context* _in_context, int* _out_focus)
{
    if (!_in_context)
    {
        printf("%s(%s) : Context is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_context->window)
    {
        printf("%s(%s) : Context window is NULL.\n", __func__, __FILE__);
        return (0);
    }

    *_out_focus = glfwGetWindowAttrib(_in_context->window, GLFW_FOCUSED);

    return (1);
}

int mgeContextShouldClose(Context* _in_context, int* _out_should_close)
{
    if (!_in_context)
    {
        printf("%s(%s) : Context is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_context->window)
    {
        printf("%s(%s) : Context window is NULL.\n", __func__, __FILE__);
        return (0);
    }

    *_out_should_close = glfwWindowShouldClose(_in_context->window);

    return (1);
}