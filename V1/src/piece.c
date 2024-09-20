#include "game/piece.h"

char mcg_TypeToChar(Type t)
{
    switch (t)
    {
    case CHESS_TYPE_PAWN:
        return 'P';
    
    case CHESS_TYPE_ROOK:
        return 'R';

    case CHESS_TYPE_KNIGHT:
        return 'H';

    case CHESS_TYPE_BISHOP:
        return 'B';

    case CHESS_TYPE_QUEEN:
        return 'Q';

    case CHESS_TYPE_KING:
        return 'K';

    default:
        return ' ';
    }
}

char mcg_ColorToChar(Color c)
{
    switch (c)
    {
    case CHESS_COLOR_BLACK:
        return 'B';
    
    case CHESS_COLOR_WHITE:
        return 'W';
    
    default:
        return ' ';
    }
}