#ifndef H_CHESS_GAME
#define H_CHESS_GAME


#include "core/renderer.h"
#include "core/texture.h"

#include "game/board.h"

#include "vendor/glad/glad.h"
#include "vendor/GLFW/glfw3.h"

typedef enum GameType
{
    GAMETYPE_CONSOLE,
    GAMETYPE_2D,
    GAMETYPE_3D,
} GameType;

typedef struct Game
{
    GameType type;
    Board* board;

    Color current_turn;
    Color winner;

    int running;

    Position selected[2];
    int select_index;

    GLFWwindow *window;
    int window_width;
    int window_height;
    int fullscreen;

    Renderer *renderer;

    Mesh *board_mesh;
    Mesh *pieces_mesh;
    
    Position *piece_position;

    Texture *textures;
    int n_textures;

    float board_offset_x;
    float board_offset_y;
    float board_tile_width;
    float board_tile_height;
} Game;

Game* mcg_CreateGame();
void mcg_GameLoop(Game* game);
void mcg_ClearGame(Game* game);

#endif