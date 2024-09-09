#ifndef H_CHESS_RENDERER
#define H_CHESS_RENDERER

#define BATCH_SIZE 100
#define MAX_VERTEX_PER_MESH 512
#define MAX_BATCH 16

#include "core/mesh.h"

typedef struct {
    unsigned int count;
    unsigned int instanceCount;
    unsigned int firstIndex;
    int          baseVertex;
    unsigned int baseInstance;
} DrawCommand;

typedef struct
{
    MeshType type;
    int material;

    Mesh* objects[BATCH_SIZE];
    int size;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int dibo;
    unsigned int matrix_ssbo;
    unsigned int textures_ssbo;

    int vbo_offset;
    int ebo_offset;

    unsigned int shader;
} RenderPool;

typedef struct
{
    RenderPool batches[MAX_BATCH];
    int count;

    unsigned int shader;

} Renderer;

void mcg_InitRenderPool(RenderPool* pool, int material, MeshType mode);
void mcg_FreeRenderPool(RenderPool* pool);

Renderer* mcg_CreateRenderer();
void mcg_FreeRenderer(Renderer* renderer);

void mcg_PushModel(Renderer* renderer, Mesh* object, int material);
DrawCommand* mcg_CreateDrawCommands(RenderPool* pool, int* count);

void mcg_Render(Renderer* renderer);

#endif