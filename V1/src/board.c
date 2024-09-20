#include "core/utils.h"

#include "game/board.h"
#include "game/piece_check.h"

#include <stdlib.h>
#include <stdio.h>

Board *mcg_CreateBoard()
{
    Board *board;
    
    board = (Board*) malloc( sizeof(Board) );

    if (board == NULL)
    {
        exit(EXIT_FAILURE);
    }

    board->array = (Piece **) malloc( sizeof(Piece *) * 8);

    if (board->array == NULL)
    {
        exit(EXIT_FAILURE);
    }

    board->size = 8;

    for (int i = 0; i < 8; i++)
    {
        board->array[i] = (Piece *) malloc( sizeof(Piece) * 8);

        if (board->array[i] == NULL)
        {
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < 8; j++)
        {
            board->array[i][j].color = CHESS_COLOR_NONE;
            board->array[i][j].type = CHESS_TYPE_EMPTY;
            board->array[i][j].moved = 0;
        }
    }

    return (board);
}

void mcg_SetPiece(Board* board, Piece piece, int x, int y)
{
    board->array[y][x] = piece;
}

Piece mcg_GetPiece(Board* board, int x, int y)
{
    return board->array[y][x];
}

void mcg_DeletePiece(Board* board, int x, int y)
{
    board->array[y][x].color = CHESS_COLOR_NONE;
    board->array[y][x].type = CHESS_TYPE_EMPTY;
}

Piece mcg_MovePiece(Board* board, int x, int y, int dx, int dy)
{
    Piece eaten = mcg_GetPiece(board, dx, dy);
    Piece p = mcg_GetPiece(board, x, y);
    p.moved += 1;
    mcg_DeletePiece(board, x, y);
    mcg_SetPiece(board, p, dx, dy);

    return eaten;
}

Position mcg_GetKing(Board* board, Color color)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // if wrong color or empty tile, we skip to next piece
            if (mcg_GetPiece(board, i, j).color == color)
            {
                if (mcg_GetPiece(board, i, j).type == CHESS_TYPE_KING)
                {
                    return (Position) {i, j};
                }   
            }
        }
    }

    return (Position) {-8, -8};
}

void mcg_ChangePieceType(Board* board, Type t, int x, int y)
{
    board->array[y][x].type = t;
}

void mcg_InitBoard(Board *board)
{
    Type court[8] = {CHESS_TYPE_ROOK, CHESS_TYPE_KNIGHT, CHESS_TYPE_BISHOP, CHESS_TYPE_QUEEN, CHESS_TYPE_KING, CHESS_TYPE_BISHOP, CHESS_TYPE_KNIGHT, CHESS_TYPE_ROOK};

    for (int i = 0; i < 8; i++)
    {
        mcg_SetPiece(board, (Piece){ court[i], CHESS_COLOR_BLACK, 0 }, i, 0);
        mcg_SetPiece(board, (Piece){ CHESS_TYPE_PAWN, CHESS_COLOR_BLACK, 0 }, i, 1);

        mcg_SetPiece(board, (Piece){ CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0 }, i, board->size - 2);
        mcg_SetPiece(board, (Piece){ court[i], CHESS_COLOR_WHITE, 0 }, i, board->size - 1);
    }
}

void mcg_ClearBoard(Board* board)
{
    for (int i = 0; i < 8; i++)
    {
        free(board->array[i]);
    }

    free(board->array);
    free(board);
}

int mcg_IsCheck(Board* board, Position king)
{
    Position attacker_pieces[16];

    int n_attack = 0;

    Type promote_options[4] = {CHESS_TYPE_BISHOP, CHESS_TYPE_KNIGHT, CHESS_TYPE_QUEEN, CHESS_TYPE_ROOK};

    // TODO : check for the promotion

    Color defender_color = mcg_GetPiece(board, king.x, king.y).color;
    Color attacker_color = !defender_color;

    Piece piece_a;
    Piece piece_b;

    Position pos_a;
    Position pos_b;


    // we get the position of the attacker pieces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            // if wrong color or empty tile, we skip to next piece
            if (mcg_GetPiece(board, i, j).color == attacker_color && mcg_GetPiece(board, i, j).type != CHESS_TYPE_EMPTY)
            {
                attacker_pieces[n_attack] = (Position) {i, j};
                n_attack++;    
            }
        }
    }

    // for each pieces we check if it can capture the king
    for (int k = 0; k < n_attack; k++)
    {
        // we store the original pieces
        piece_a = mcg_GetPiece(board, attacker_pieces[k].x, attacker_pieces[k].y);

        pos_a = (Position) {attacker_pieces[k].x, attacker_pieces[k].y};

        switch (piece_a.type)
        {
        case CHESS_TYPE_PAWN:
            if (mcg_PawnMovement(board, pos_a, king))
                return (1);

            PositionArray moves = mcg_GeneratePawnPositions(board, pos_a);
            
            for (int i = 0; i < mcg_SizePositionArray(&moves); i++)
            {
                pos_b = mcg_GetPositionArray(&moves, i);
                piece_b = mcg_GetPiece(board, pos_b.x, pos_b.y);


                if (pos_b.y == 0 || pos_b.y == 7)
                {
                    mcg_MovePiece(board, pos_a.x, pos_a.y, pos_b.x, pos_b.y);

                    for (int j = 0; j < 4; j++)
                    {
                        mcg_ChangePieceType(board, promote_options[j], pos_b.x, pos_b.y);
                        switch(promote_options[j])
                        {
                        case CHESS_TYPE_ROOK:
                            if (mcg_RookMovement(board, pos_b, king))
                            {
                                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                                return (1);
                            }
                            break;

                        case CHESS_TYPE_KNIGHT:
                            if (mcg_KnightMovement(board, pos_b, king))
                            {
                                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                                return (1);
                            }
                            break;

                        case CHESS_TYPE_BISHOP:
                            if (mcg_BishopMovement(board, pos_b, king))
                            {
                                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                                return (1);
                            }
                            break;

                        case CHESS_TYPE_QUEEN:
                            if(mcg_QueenMovement(board, pos_b, king))
                            {
                                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                                return (1);
                            }
                            break;

                        case CHESS_TYPE_KING:
                            if(mcg_KingMovement(board, pos_b, king))
                            {
                                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                                return (1);
                            }
                            break;

                        default:
                            break;
                        }


                        mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                        mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                    }
                }
            }

            mcg_FreePositionArray(&moves);

            break;

        case CHESS_TYPE_ROOK:
            if (mcg_RookMovement(board, pos_a, king))
            {
                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                return (1);
            }
            break;

        case CHESS_TYPE_KNIGHT:
            if (mcg_KnightMovement(board, pos_a, king))
            {
                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                return (1);
            }
            break;

        case CHESS_TYPE_BISHOP:
            if (mcg_BishopMovement(board, pos_a, king))
            {
                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                return (1);
            }
            break;

        case CHESS_TYPE_QUEEN:
            if(mcg_QueenMovement(board, pos_a, king))
            {
                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                return (1);
            }
            break;

        case CHESS_TYPE_KING:
            if (mcg_KingMovement(board, pos_a, king))
            {
                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
                return (1);
            }
            break;
        
        default:
            break;
        }
    }

    return (0);
}

int mcg_IsCheckmate(Board* board, Color player)
{
    Position defender_pieces[16];
    Position defender_king;

    Piece piece_a;
    Position pos_a;

    int n_defense = 0;
    
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (mcg_GetPiece(board, i, j).color == player)
            {
                if (mcg_GetPiece(board, i, j).type == CHESS_TYPE_KING)
                {   
                    defender_king = (Position) {i, j};
                }

                defender_pieces[n_defense] = (Position) {i, j};
                n_defense++;
            }
        }
    }
    
    if (!mcg_IsCheck(board, defender_king))
    {
        return (0);
    }

    for (int k = 0; k < n_defense; k++)
    {
        piece_a = mcg_GetPiece(board, defender_pieces[k].x, defender_pieces[k].y);
        pos_a = defender_pieces[k];

        PositionArray array;

        array = mcg_GeneratePositions(board, pos_a);

        for (int i = 0; i < mcg_SizePositionArray(&array); i++)
        {
            Position pos_b = mcg_GetPositionArray(&array, i);
            Piece piece_b = mcg_GetPiece(board, pos_b.x, pos_b.y);

            mcg_MovePiece(board, pos_a.x, pos_a.y, pos_b.x, pos_b.y);

            if (!mcg_IsCheck(board, defender_king))
            {
                mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
                mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);

                return (0);
            }

            mcg_SetPiece(board, piece_a, pos_a.x, pos_a.y);
            mcg_SetPiece(board, piece_b, pos_b.x, pos_b.y);
        }

        mcg_FreePositionArray(&array);
    }

    return (1);
}

int mcg_CanCastle(Board* board, Position king_pos, Position rook_pos)
{
    Piece king = mcg_GetPiece(board, king_pos.x, king_pos.y);
    Piece rook = mcg_GetPiece(board, rook_pos.x, rook_pos.y);

    Position ray;

    if (king.color != rook.color)
        return (0);

    if (king.moved || rook.moved)
        return (0);

    if (mcg_IsCheck(board, king_pos))
        return (0);

    ray = mcg_HitRay(board, king_pos, (Position) {mcg_Sign(rook_pos.x - king_pos.x), 0 } );

    if (ray.x != rook_pos.x || ray.y != rook_pos.y)
        return (0);

    int normalized_dir = mcg_NormalizedRange(mcg_Sign(rook_pos.x - king_pos.x));
    mcg_Castle(board, king_pos, rook_pos);

    if (mcg_IsCheck(board, king_pos))
    {

        if (normalized_dir)
        {
            mcg_MovePiece(board, 6, king_pos.y, king_pos.x, king_pos.y);
            mcg_MovePiece(board, 5, rook_pos.y, rook_pos.x, rook_pos.y);
        }
        else
        {
            mcg_MovePiece(board, 2, king_pos.y, king_pos.x, king_pos.y);
            mcg_MovePiece(board, 3, rook_pos.y, rook_pos.x, rook_pos.y);
        }

        return (0);
    }

    if (normalized_dir)
    {
        mcg_MovePiece(board, 6, king_pos.y, king_pos.x, king_pos.y);
        mcg_MovePiece(board, 5, rook_pos.y, rook_pos.x, rook_pos.y);
    }
    else
    {
        mcg_MovePiece(board, 2, king_pos.y, king_pos.x, king_pos.y);
        mcg_MovePiece(board, 3, rook_pos.y, rook_pos.x, rook_pos.y);
    }

    return (1);
}

void mcg_Castle(Board* board, Position king_pos, Position rook_pos)
{
    int normalized_dir = mcg_NormalizedRange(mcg_Sign(rook_pos.x - king_pos.x));

    if (normalized_dir)
    {
        mcg_MovePiece(board, king_pos.x, king_pos.y, 6, king_pos.y);
        mcg_MovePiece(board, rook_pos.x, rook_pos.y, 5, rook_pos.y);
    }
    else
    {
        mcg_MovePiece(board, king_pos.x, king_pos.y, 2, king_pos.y);
        mcg_MovePiece(board, rook_pos.x, rook_pos.y, 3, rook_pos.y);
    }
}

void mcg_PrintBoard(Board* board)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board->array[i][j].type != CHESS_TYPE_EMPTY)
                printf("%c%c ", mcg_TypeToChar(board->array[i][j].type), mcg_ColorToChar(board->array[i][j].color));
            else
                printf("   ");
        }
        printf("\n");
    }
    printf("\n");
}