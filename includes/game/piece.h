#ifndef H_CHESS_PIECE
#define H_CHESS_PIECE

typedef enum
{   
    CHESS_TYPE_PAWN = 0,
    CHESS_TYPE_ROOK = 1,
    CHESS_TYPE_KNIGHT = 2,
    CHESS_TYPE_BISHOP = 3,
    CHESS_TYPE_QUEEN = 4,
    CHESS_TYPE_KING = 5,
    CHESS_TYPE_EMPTY = 6
} Type;

typedef enum
{
    CHESS_COLOR_WHITE = 0,
    CHESS_COLOR_BLACK = 1,
    CHESS_COLOR_NONE = 2
} Color;

char mcg_TypeToChar(Type t);
char mcg_ColorToChar(Color c);

typedef struct
{
    Type type;
    Color color;
    int moved;
} Piece;

#endif