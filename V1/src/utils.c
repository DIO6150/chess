#include <stdlib.h>

#include "core/utils.h"


int mcg_Sign(int x) {
    return (x > 0) - (x < 0);
}

void mcg_ClearInputStream()
{
    while ( getchar() != '\n' );
}

int mcg_SymetrizeRange(int x)
{
    return (x * 2 - 1);
}

int mcg_NormalizedRange(int x)
{
    return ((x + 1) / 2);
}

float mcg_SymetrizeRange1f(float x)
{
    return (x * 2.0f - 1.0f);
}

float mcg_NormalizedRange1f(float x)
{
    return ((x + 1.0f) / 2.0f);
}

PositionArray mcg_CreatePositionArray(int size)
{
    PositionArray array;

    array.array = malloc(sizeof(Position) * size);

    if (array.array == NULL)
    {
        exit(EXIT_FAILURE);
    }

    array.size = size;
    array.n = 0;

    for (int i = 0; i < size; i++)
    {
        array.array[i] = (Position) {0, 0};
    }

    return array;
}

void mcg_FreePositionArray(PositionArray* array)
{
    free(array->array);
}

void mcg_PushPositionArray(PositionArray* array, Position pos)
{
    if (array->n < array->size)
    {
        array->array[array->n] = pos;
        array->n++;
    }
}

int mcg_SizePositionArray(PositionArray* array)
{
    return array->n;
}

Position mcg_GetPositionArray(PositionArray* array, int index)
{
    if (0 <= index && index < array->n)
    {
        return array->array[index];
    }

    return (Position) {-8, -8};
}