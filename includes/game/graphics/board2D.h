#ifndef H_CHESS_BOARD2D
#define H_CHESS_BOARD2D

#include "core/mesh.h"

#include "game/piece.h"


Mesh mcg_CreateBoard2D();
void mcg_FillBoard2D(Piece** board);

#endif