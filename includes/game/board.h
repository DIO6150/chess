#ifndef H_CHESS_BOARD
#define H_CHESS_BOARD

#include "core/utils.h"
#include "game/piece.h"

typedef struct
{
    Piece **array;
    int size;
} Board;

Board *mcg_CreateBoard();

void mcg_SetPiece(Board* board, Piece piece, int x, int y);
Piece mcg_GetPiece(Board* board, int x, int y);
void mcg_DeletePiece(Board* board, int x, int y);
void mcg_MovePiece(Board* board, int x, int y, int dx, int dy);
void mcg_ChangePieceType(Board* board, Type t, int x, int y);

void mcg_InitBoard(Board *board);
void mcg_ClearBoard(Board *board);

Position mcg_GetKing(Board* board, Color color);

int mcg_CanCastle(Board* board, Position king_pos, Position rook_pos);
void mcg_Castle(Board* board, Position king_pos, Position rook_pos);

int mcg_IsCheck(Board* board, Position king);
int mcg_IsCheckmate(Board* board, Color player);

Position mcg_IsMoveAllowed(Board* board, Position position_a, Position position_b);

void mcg_PrintBoard(Board* board);

#endif