#ifndef H_MGE_RENDERER
#define H_MGE_RENDERER

#include "core/mesh.h"
#include "core/shader.h"

/*   DRAW COMMANDS   */


// Data
typedef struct
{
    unsigned int count;
    unsigned int instance_count;
    unsigned int first_index;
    int base_vertex;
    unsigned int base_instance;
} DrawCommand;



/*   BATCH   */


// Data
typedef struct
{
    int batch_id; // The unique id of the batch

    Shader* shader; // Shader used by the entire batch
    
    int mesh_max_count; // max number of mesh
    int vertex_max_count; // number of max verticies in the vbo
    int index_max_count; // number of max indicies in the ebo

    int mesh_count; // number of mesh in the batch
    int vertex_count; // number of verticies in the vbo
    int index_count; // number of indicies in the ebo

    unsigned int vao; // Vertex Array
    unsigned int vbo; // Vertex Buffer
    unsigned int ebo; // Element Buffer
    unsigned int dibo; // Draw Indirect Buffer

    unsigned int matrix_ssbo; // Matrix Shader Storage Buffer

} Batch;

// Functions
int mgeBatchInit(int _in_batch_id, Batch** _out_batch);
int mgeBatchFree(Batch* _in_batch);

int mgeBatchBindBuffers(Batch* _in_batch);
int mgeBatchUnbindBuffers();
int mgeBatchResize(Batch* _in_batch, int _in_mesh_count, int _in_vertex_count, int _in_index_count);
int mgeBatchPushMesh(Batch* _in_batch, Mesh* _in_mesh);

int mgeBatchReset(Batch* _in_batch);



/*   RENDERER   */

#define MGE_BATCH_MAX_COUNT 16

// Data
typedef struct
{
    Batch* batches[MGE_BATCH_MAX_COUNT];
    int batch_count;

    mat4 projection;
    mat4 view;

    Texture2D** textures;
    int textures_capacity;
    int texture_count;

    TextureObject* atlas;

    // TODO : Add a camera field
    vec3 camera_up;
    vec3 camera_front;
    vec3 camera_position;
} Renderer;

// Functions
int mgeRendererInit(Renderer** _out_renderer);
int mgeRendererFree(Renderer* _in_renderer);

int mgeAddTexture(Renderer* _in_renderer, Texture2D* _in_texture);
int mgeCreateAtlas(Renderer* _in_renderer);

int mgeRendererSetBatchShader(Renderer* _in_renderer, int _in_batch_id, Shader* _in_shader);

int mgeRendererPushMesh(Renderer* _in_renderer, int _in_batch_id, Mesh* _in_mesh);

int mgeRender(Renderer* _in_renderer);

#endif