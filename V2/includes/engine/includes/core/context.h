/*
    Simple Wrapper Class for glfwwindow
*/

// TODO : Add callbacks, for debug, for resize, etc...

#ifndef H_MGE_CONTEXT
#define H_MGE_CONTEXT

#include "glad/glad.h"
#include "GLFW/glfw3.h"

typedef struct
{
    GLFWwindow* window;
    const char* name;
    int width;
    int height;
} Context;


int mgeContextInit(char* _in_name, int _in_width, int _in_height, Context** _out_context);
int mgeContextFree(Context* _in_context);

int mgeContextShouldClose(Context* _in_context, int* _out_should_close);
int mgeContextFocused(Context* _in_context, int* _out_focus);

#endif