#ifndef CHESS_UTILS
#define CHESS_UTILS

#include <stdio.h>

int mcg_Sign(int x);
void mcg_ClearInputStream();
int mcg_SymetrizeRange(int x);
int mcg_NormalizedRange(int x);
float mcg_SymetrizeRange1f(float x);
float mcg_NormalizedRange1f(float x);

typedef struct Position
{
    int x;
    int y;
} Position;

typedef struct PositionArray
{
    Position* array;
    int size;
    int n;
} PositionArray;

PositionArray mcg_CreatePositionArray(int size);
void mcg_FreePositionArray(PositionArray* array);

void mcg_PushPositionArray(PositionArray* array, Position pos);
int mcg_SizePositionArray(PositionArray* array);
Position mcg_GetPositionArray(PositionArray* array, int index);

#endif