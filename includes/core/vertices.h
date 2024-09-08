#ifndef H_CHESS_VERTICES
#define H_CHESS_VERTICES

typedef struct
{
    float x;
    float y;
    float z;

    float r;
    float g;
    float b;

    float u;
    float v;

    float tx;
    float ty;
    float tz;
} Vertices3D;

typedef struct
{
    float x;
    float y;

    float r;
    float g;
    float b;

    float u;
    float v;
} Vertices2D;

#endif