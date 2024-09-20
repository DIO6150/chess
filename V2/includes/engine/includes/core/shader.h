#ifndef H_MGE_SHADER
#define H_MGE_SHADER

typedef struct
{
    unsigned int program;
    unsigned int vertex_shader;
    unsigned int fragment_shader;

    char* vertex_path;
    char* fragment_path;

    char* vertex_source;
    char* fragment_source;
} Shader;

int mgeShaderLoad(char* _in_vertex_path, char* _in_fragment_path, Shader** _out_shader);

int mgeShaderCreate(char* _in_vertex_source, char* _in_fragment_source, Shader** _out_shader);
int mgeShaderDelete(Shader* _in_shader);

int mgeShaderCompile(Shader* _in_shader);

int mgeShaderUse(Shader* _in_shader);
int mgeShaderCancel();

#endif