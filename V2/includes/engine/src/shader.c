#include "core/shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "glad/glad.h"

int mgeShaderLoad(char* _in_vertex_path, char* _in_fragment_path, Shader **_out_shader)
{
    FILE* fvertex;
    FILE* ffragment;

    
    char vertex_source[1000];
    char fragment_source[1000];

    char line[256];

    fvertex = fopen(_in_vertex_path, "r");

    if (!fvertex)
    {
        printf("%s(%s) : File doesn't exist : %s.\n", __func__, __FILE__, _in_vertex_path);
        return (0);
    }

    ffragment = fopen(_in_fragment_path, "r");

    if (!ffragment)
    {
        fclose(fvertex);
        printf("%s(%s) : File doesn't exist : %s.\n", __func__, __FILE__, _in_fragment_path);
        return (0);
    }

    while (fgets(line, sizeof(line), fvertex))
    {
        strcat(vertex_source, line);
    }

    while (fgets(line, sizeof(line), ffragment))
    {
        strcat(fragment_source, line);
    }

    fclose(fvertex);
    fclose(ffragment);

    if (!mgeShaderCreate(vertex_source, fragment_source, _out_shader))
    {
        printf("%s(%s) : Could'nt create the shader.\n", __func__, __FILE__);
        return (0);
    }

    (*_out_shader)->vertex_path = _in_vertex_path;
    (*_out_shader)->fragment_path = _in_fragment_path;

    return (1);
}

int mgeShaderCreate(char* _in_vertex_source, char* _in_fragment_source, Shader** _out_shader)
{
    (*_out_shader) = (Shader*) malloc(sizeof(Shader));

    (*_out_shader)->vertex_path = NULL;
    (*_out_shader)->fragment_path = NULL;

    (*_out_shader)->vertex_source = _in_vertex_source;
    (*_out_shader)->fragment_source = _in_fragment_source;

    if (!mgeShaderCompile(*_out_shader))
    {
        free((*_out_shader));
        *_out_shader = NULL;
        printf("%s(%s) : Could'nt compile shader, aborting.\n", __func__, __FILE__);
        return (0);
    }

    return (1);
}

int mgeShaderDelete(Shader* _in_shader)
{
    if (!_in_shader)
    {
        printf("%s(%s) : Shader is NULL.\n", __func__, __FILE__);
        return (0);
    }

    glDeleteShader(_in_shader->vertex_shader);
    glDeleteShader(_in_shader->fragment_shader);
    glDeleteProgram(_in_shader->program);

    free(_in_shader);

    return (1);
}

int mgeShaderCompile(Shader* _in_shader)
{

    if (!_in_shader)
    {
        printf("%s(%s) : Shader is NULL :\n", __func__, __FILE__);

        return (0);
    }

    if (!_in_shader->vertex_source)
    {
        printf("%s(%s) : Vertex Source is NULL :\n", __func__, __FILE__);

        return (0);
    }

    if (!_in_shader->fragment_source)
    {
        printf("%s(%s) : Fragment Source is NULL :\n", __func__, __FILE__);

        return (0);
    }

    char info_log[1024];
    int success;
    unsigned int vertex_shader;
    unsigned int fragment_shader;

    //const char* vertex_source = _in_shader->vertex_source;

    const char* vertex_source = _in_shader->vertex_source;

    const char* fragment_source = _in_shader->fragment_source;

    // Vertex Shader Step

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);
    
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        printf("%s(%s) : Error when compiling vertex shader :\n%s\n", __func__, __FILE__, info_log);

        return (0);
    }

    // Fragment Shader Step

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        printf("%s(%s) : Error when compiling fragment shader :\n%s\n", __func__, __FILE__, info_log);

        return (0);
    }

    // Shader Program Step

    _in_shader->program = glCreateProgram();

    glAttachShader(_in_shader->program, vertex_shader);
    glAttachShader(_in_shader->program, fragment_shader);
    glLinkProgram(_in_shader->program);

    glGetProgramiv(_in_shader->program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(_in_shader->program, 512, NULL, info_log);
        printf("%s(%s) : Error when compiling shader program :\n%s\n", __func__, __FILE__, info_log);

        return (0);
    }

    return (1);
}

int mgeShaderUse(Shader* _in_shader)
{
    if (!_in_shader)
    {
        printf("%s(%s) : Shader is NULL.\n", __func__, __FILE__);
        return (0);
    }

    glUseProgram(_in_shader->program);
    return (1);
}

int mgeShaderCancel()
{
    glUseProgram(0);
    return (1);
}