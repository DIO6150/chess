#include <math.h>

#include "core/utils.h"
#include "game/piece_check.h"

Position mcg_HitRay(Board* board, Position pos_a, Position vector)
{
    Position ray = pos_a;

    do
    {
        if (ray.x + vector.x > 7 || ray.x + vector.x < 0 || ray.y + vector.y > 7 || ray.y + vector.y < 0)
        {
            break;
        }

        ray.x += vector.x;
        ray.y += vector.y;


    } while (mcg_GetPiece(board, ray.x, ray.y).type == CHESS_TYPE_EMPTY);

    return ray;
}

void mcg_TraceRay(Board* board, Position pos_a, Position vector, PositionArray *array)
{
    Position ray = pos_a;

    do
    {
        if (ray.x + vector.x > 7 || ray.x + vector.x < 0 || ray.y + vector.y > 7 || ray.y + vector.y < 0)
        {
            break;
        }

        ray.x += vector.x;
        ray.y += vector.y;

        mcg_PushPositionArray(array, ray);


    } while (mcg_GetPiece(board, ray.x, ray.y).type == CHESS_TYPE_EMPTY);
}

int mcg_PawnMovement(Board* board, Position src, Position dest)
{
    Piece piece_a = mcg_GetPiece(board, src.x, src.y);
    Color color_a = piece_a.color;

    if (piece_a.type == CHESS_TYPE_PAWN)
    {
        if (color_a == mcg_GetPiece(board, dest.x, dest.y).color)
            return (0);

        if (!piece_a.moved)
        {
            if (src.x == dest.x && src.y + mcg_SymetrizeRange(color_a) * 2 == dest.y)
            {
                if (mcg_GetPiece(board, dest.x, dest.y - mcg_SymetrizeRange(color_a)).type == CHESS_TYPE_EMPTY)
                {
                    if (mcg_GetPiece(board, dest.x, dest.y).type == CHESS_TYPE_EMPTY)
                    return (1);
                }
            }
        }

        if (src.x == dest.x && src.y + mcg_SymetrizeRange(color_a) == dest.y)
        {
            if (mcg_GetPiece(board, dest.x, dest.y).type == CHESS_TYPE_EMPTY)
                return (1);
        }

        if (abs(dest.x - src.x) == 1 && dest.y - src.y == mcg_SymetrizeRange(color_a))
        {
            if (mcg_GetPiece(board, dest.x, dest.y).type != CHESS_TYPE_EMPTY && mcg_GetPiece(board, dest.x, dest.y).color != color_a)
                return (1);
        }
    }

    return (0);
}

int mcg_RookMovement(Board* board, Position src, Position dest)
{
    Piece piece_a = mcg_GetPiece(board, src.x, src.y);
    Color color_a = piece_a.color;

    Position ray = src;

    if (piece_a.type == CHESS_TYPE_ROOK)
    {
        if (color_a == mcg_GetPiece(board, dest.x, dest.y).color)
            return (0);

        if (src.x == dest.x)
        {
            ray = mcg_HitRay(board, ray, (Position) {0, mcg_Sign( dest.y - src.y )});
        }
        else if (src.y == dest.y)
        {
            ray = mcg_HitRay(board, ray, (Position) {mcg_Sign( dest.x - src.x ), 0});
        }

        if (ray.x == dest.x && ray.y == dest.y)
        {
            return (1);
        }
    }

    return (0);
}

int mcg_KnightMovement(Board* board, Position src, Position dest)
{
    Piece piece_a = mcg_GetPiece(board, src.x, src.y);
    Color color_a = piece_a.color;

    if (piece_a.type == CHESS_TYPE_KNIGHT)
    {
        if (color_a == mcg_GetPiece(board, dest.x, dest.y).color)
            return (0);

        if (abs(src.x - dest.x) == 2 && abs(src.y - dest.y) == 1)
        {
            return (1);
        }

        else if (abs(src.x - dest.x) == 1 && abs(src.y - dest.y) == 2)
        {
            return (1);
        }
    }

    return (0);
}

int mcg_BishopMovement(Board* board, Position src, Position dest)
{
    Piece piece_a = mcg_GetPiece(board, src.x, src.y);
    Color color_a = piece_a.color;

    Position ray = src;

    if (piece_a.type == CHESS_TYPE_BISHOP)
    {
        if (color_a == mcg_GetPiece(board, dest.x, dest.y).color)
            return (0);

        if (abs(dest.x - src.x) == abs(dest.y - src.y))
        {
            ray = mcg_HitRay(board, src, (Position) { mcg_Sign( dest.x - src.x ), mcg_Sign( dest.y - src.y ) });

            if (ray.x == dest.x && ray.y == dest.y)
            {
                return (1);
            }
        }
    }

    return (0);
}
int mcg_QueenMovement(Board* board, Position src, Position dest)
{
    Piece piece_a = mcg_GetPiece(board, src.x, src.y);
    Color color_a = piece_a.color;

    if (piece_a.type == CHESS_TYPE_QUEEN)
    {
        if (color_a == mcg_GetPiece(board, dest.x, dest.y).color)
            return (0);

        PositionArray ray_path = mcg_CreatePositionArray(9);

        if (src.x == dest.x)
        {
            mcg_TraceRay(board, src, (Position) {0, mcg_Sign( dest.y - src.y )}, &ray_path);
        }
        else if (src.y == dest.y)
        {
            mcg_TraceRay(board, src, (Position) {mcg_Sign( dest.x - src.x ), 0}, &ray_path);
        }

        for (int i = 0; i < mcg_SizePositionArray(&ray_path); i++)
        {
            Position temp = mcg_GetPositionArray(&ray_path, i);
            if (temp.x == dest.x && temp.y == dest.y)
            {
                mcg_FreePositionArray(&ray_path);
                return (1);
            }
        }

        if (abs(dest.x - src.x) == abs(dest.y - src.y))
        {
            mcg_TraceRay(board, src, (Position) { mcg_Sign( dest.x - src.x ), mcg_Sign( dest.y - src.y ) }, &ray_path);

            for (int i = 0; i < mcg_SizePositionArray(&ray_path); i++)
            {
                Position temp = mcg_GetPositionArray(&ray_path, i);
                if (temp.x == dest.x && temp.y == dest.y)
                {
                    mcg_FreePositionArray(&ray_path);
                    return (1);
                }
            }
        }

        mcg_FreePositionArray(&ray_path);
    }

    return (0);
}

int mcg_KingMovement(Board* board, Position src, Position dest)
{
    Piece piece_a = mcg_GetPiece(board, src.x, src.y);
    Color color_a = piece_a.color;

    if (piece_a.type == CHESS_TYPE_KING)
    {
        if (color_a == mcg_GetPiece(board, dest.x, dest.y).color)
            return (0);

        if (abs(dest.x - src.x) == 1 || abs(dest.y - src.y) == 1)
        {
            return (1);
        }
    }

    return (0);
}

int mcg_PieceMovement(Board* board, Position src, Position dest)
{

    if (src.x > 7 || src.x < 0 || src.y > 7 || src.y < 0 || dest.x > 7 || dest.x < 0 || dest.y > 7 || dest.y < 0)
        return (0);

    switch (mcg_GetPiece(board, src.x, src.y).type)
    {
    case CHESS_TYPE_PAWN:
        return (mcg_PawnMovement(board, src, dest));

    case CHESS_TYPE_ROOK:
        return (mcg_RookMovement(board, src, dest));

    case CHESS_TYPE_KNIGHT:
        return (mcg_KnightMovement(board, src, dest));
    
    case CHESS_TYPE_BISHOP:
        return (mcg_BishopMovement(board, src, dest));

    case CHESS_TYPE_QUEEN:
        return (mcg_QueenMovement(board, src, dest));

    case CHESS_TYPE_KING:
        return (mcg_KingMovement(board, src, dest));

    default:
        break;
    }

    return (0);
}

PositionArray mcg_GeneratePawnPositions(Board* board, Position src)
{
    PositionArray array;
    
    Piece piece_a;
    Piece piece_b;

    Position diagonal;
    Color color_a;
    
    int range[] = {-1, 1};

    array = mcg_CreatePositionArray(32);
    
    piece_a = mcg_GetPiece(board, src.x, src.y);
    color_a = piece_a.color;

    mcg_PushPositionArray(&array, (Position) { src.x, src.y + mcg_SymetrizeRange(color_a) } );

    if (!piece_a.moved)
    {
        if (mcg_GetPiece(board, src.x, src.y + mcg_SymetrizeRange(color_a)).type == CHESS_TYPE_EMPTY)
        {
            if (mcg_GetPiece(board, src.x, src.y + mcg_SymetrizeRange(color_a) * 2).type == CHESS_TYPE_EMPTY)
                mcg_PushPositionArray(&array, (Position) { src.x, src.y + mcg_SymetrizeRange(color_a) * 2} );
        }
    }

    for (int i = 0; i < 2; i++)
    {
        diagonal = (Position){ src.x + range[i], src.y + mcg_SymetrizeRange(color_a)};
        piece_b = mcg_GetPiece(board, diagonal.x, diagonal.y);

        if (diagonal.x < 0 || diagonal.x > 7 || diagonal.y < 0 || diagonal.y > 7)
            continue;

        if (piece_b.type != CHESS_TYPE_EMPTY && piece_b.color != color_a)
        {
            mcg_PushPositionArray(&array, (Position) { src.x + diagonal.x, src.y + diagonal.y } );
        }
    }

    return array;
}

PositionArray mcg_GenerateRookPositions(Board* board, Position src)
{
    PositionArray array;

    Position ray;

    array = mcg_CreatePositionArray(32);

    Position range[] = {
        {0, 1}, {0, -1},
        {1, 0}, {-1, 0}
    };

    ray = src;

    for (int i = 0; i < 8; i++)
    {
        do
        {          
            if (ray.x + range[i].x > 7 || ray.x + range[i].x < 0 || ray.y + range[i].y > 7 || ray.y + range[i].y < 0)
            {    
                break;
            }

            if (mcg_GetPiece(board, ray.x, ray.y).color == mcg_GetPiece(board, src.x, src.y).color)
                continue;

            mcg_PushPositionArray(&array, ray);

            ray.x += range[i].x;
            ray.y += range[i].y;

        } while (mcg_GetPiece(board, ray.x, ray.y).type == CHESS_TYPE_EMPTY);
    }

    return array;
}

PositionArray mcg_GenerateKnightPositions(Board* board, Position src)
{
    PositionArray array;

    array = mcg_CreatePositionArray(32);

    int c_0[] = {-1, 1};
    int c_1[] = {-2, 2};

    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            int x_0 = src.x + c_0[i];
            int y_0 = src.y + c_1[j];
            int x_1 = src.x + c_1[j];
            int y_1 = src.y + c_0[i];

            if (0 <= x_0 && x_0 <= 7 && 0 <= y_0 && y_0 <= 7)
            {
                if (mcg_GetPiece(board, x_0, y_0).color == mcg_GetPiece(board, src.x, src.y).color)
                    continue;

                mcg_PushPositionArray(&array, (Position) { x_0, y_0 });
            }
            
            if (0 <= x_1 && x_1 <= 7 && 0 <= y_1 && y_1 <= 7)
            {
                if (mcg_GetPiece(board, x_1, y_1).color == mcg_GetPiece(board, src.x, src.y).color)
                    continue;

                mcg_PushPositionArray(&array, (Position) { x_1, y_1 });
            }
        }
    }

    return array;
}

PositionArray mcg_GenerateBishopPositions(Board* board, Position src)
{
    PositionArray array;

    Position ray;

    array = mcg_CreatePositionArray(32);

    Position range[] = {
        { 1,  1}, {-1, -1},
        { 1, -1}, {-1,  1}
    };

    ray = src;

    for (int i = 0; i < 8; i++)
    {
        do
        {          
            if (ray.x + range[i].x > 7 || ray.x + range[i].x < 0 || ray.y + range[i].y > 7 || ray.y + range[i].y < 0)
            {    
                break;
            }

            if (mcg_GetPiece(board, ray.x, ray.y).color == mcg_GetPiece(board, src.x, src.y).color)
                continue;

            mcg_PushPositionArray(&array, ray);

            ray.x += range[i].x;
            ray.y += range[i].y;

        } while (mcg_GetPiece(board, ray.x, ray.y).type == CHESS_TYPE_EMPTY);
    }

    return array;
}

PositionArray mcg_GenerateQueenPositions(Board* board, Position src)
{
    PositionArray array;

    Position ray;

    array = mcg_CreatePositionArray(32);

    Position range[] = {
        { 1,  1}, {-1, -1},
        { 1, -1}, {-1,  1},
        { 0,  1}, { 0, -1},
        { 1,  0}, {-1,  0}
    };

    ray = src;

    for (int i = 0; i < 8; i++)
    {
        do
        {          
            if (ray.x + range[i].x > 7 || ray.x + range[i].x < 0 || ray.y + range[i].y > 7 || ray.y + range[i].y < 0)
            {    
                break;
            }

            if (mcg_GetPiece(board, ray.x, ray.y).color == mcg_GetPiece(board, src.x, src.y).color)
                continue;

            mcg_PushPositionArray(&array, ray);

            ray.x += range[i].x;
            ray.y += range[i].y;

        } while (mcg_GetPiece(board, ray.x, ray.y).type == CHESS_TYPE_EMPTY);
    }

    return array;
}

PositionArray mcg_GenerateKingPositions(Board* board, Position src)
{
    PositionArray array;
    
    array = mcg_CreatePositionArray(32);

    Position range[] = {
        { 1,  1}, {-1, -1},
        { 1, -1}, {-1,  1},
        { 0,  1}, { 0, -1},
        { 1,  0}, {-1,  0}
    };

    for (int i = 0; i < 8; i++)
    {
        Position temp = (Position) { src.x + range[i].x, src.y + range[i].y };

        if (temp.x > 7 || temp.x < 0 || temp.y > 7 || temp.y < 0)
            continue;

        if (mcg_GetPiece(board, temp.x, temp.y).color == mcg_GetPiece(board, src.x, src.y).color)
            continue;

        mcg_PushPositionArray(&array, temp);
    }

    return array;
}

PositionArray mcg_GeneratePositions(Board* board, Position src)
{
    PositionArray array;

    switch (mcg_GetPiece(board, src.x, src.y).type)
        {
        case CHESS_TYPE_PAWN:
            array = mcg_GeneratePawnPositions(board, src);
            break;

        case CHESS_TYPE_ROOK:
            array = mcg_GenerateRookPositions(board, src);
            break;

        case CHESS_TYPE_KNIGHT:
            array = mcg_GenerateKnightPositions(board, src);
            break;
        
        case CHESS_TYPE_BISHOP:
            array = mcg_GenerateBishopPositions(board, src);
            break;

        case CHESS_TYPE_QUEEN:
            array = mcg_GenerateQueenPositions(board, src);
            break;

        case CHESS_TYPE_KING:
            array = mcg_GenerateKingPositions(board, src);
            break;

        default:
            break;
        }

    return array;
}