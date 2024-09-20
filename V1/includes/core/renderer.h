#ifndef H_CHESS_RENDERER
#define H_CHESS_RENDERER

#include "core/mesh.h"

typedef struct {
    unsigned int count;
    unsigned int instanceCount;
    unsigned int firstIndex;
    int          baseVertex;
    unsigned int baseInstance;
} DrawCommand;

typedef enum {
    POOL_0 = 0,
    POOL_1 = 1,
    POOL_2 = 2,
    POOL_3 = 3,
    POOL_4 = 4,
    POOL_5 = 5,
    POOL_6 = 6,
    POOL_7 = 7,
    POOL_8 = 8,
    POOL_9 = 9,
    POOL_10 = 10,
    POOL_11 = 11,
    POOL_12 = 12,
    POOL_13 = 13,
    POOL_14 = 14,
    POOL_15 = 15,
} PoolID;

typedef struct
{
    PoolID pool_id;

    Mesh* *objects;
    int max_size;
    int size;

    int n_verticies;
    int n_indicies;

    int vbo_offset;
    int ebo_offset;

    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
    unsigned int dibo;
    unsigned int matrix_ssbo;
    unsigned int textures_ssbo;

    unsigned int shader;
} RenderPool;

typedef struct
{
    RenderPool* batches;
    int count;
    int max_count;

    unsigned int shader;

} Renderer;

void mcg_InitRenderPool(RenderPool* pool, int id);
void mcg_FreeRenderPool(RenderPool* pool);

Renderer* mcg_CreateRenderer();
void mcg_FreeRenderer(Renderer* renderer);

void mcg_ResizePool(RenderPool* pool, int size_offset, int max_vertex_offset, int max_index_offset);
void mcg_PushModel(Renderer* renderer, Mesh* object, PoolID id);
DrawCommand* mcg_CreateDrawCommands(RenderPool* pool, int* count);

void mcg_Render(Renderer* renderer);

#endif