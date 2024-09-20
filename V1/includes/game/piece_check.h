#ifndef H_CHESS_PIECE_CHECK
#define H_CHESS_PIECE_CHECK

#include "core/utils.h"

#include "game/board.h"
#include "game/piece.h"

Position mcg_HitRay(Board* board, Position pos_a, Position vector);
void mcg_TraceRay(Board* board, Position pos_a, Position vector, PositionArray *array);

int mcg_PawnMovement(Board* board, Position src, Position dest);
int mcg_RookMovement(Board* board, Position src, Position dest);
int mcg_KnightMovement(Board* board, Position src, Position dest);
int mcg_BishopMovement(Board* board, Position src, Position dest);
int mcg_QueenMovement(Board* board, Position src, Position dest);
int mcg_KingMovement(Board* board, Position src, Position dest);

int mcg_PieceMovement(Board* board, Position src, Position dest);

PositionArray mcg_GeneratePawnPositions(Board* board, Position src);
PositionArray mcg_GenerateRookPositions(Board* board, Position src);
PositionArray mcg_GenerateKnightPositions(Board* board, Position src);
PositionArray mcg_GenerateBishopPositions(Board* board, Position src);
PositionArray mcg_GenerateQueenPositions(Board* board, Position src);
PositionArray mcg_GenerateKingPositions(Board* board, Position src);

PositionArray mcg_GeneratePositions(Board* board, Position src);

#endif