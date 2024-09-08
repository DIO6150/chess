#ifndef H_CHESS_GAME
#define H_CHESS_GAME

#include "game/board.h"

#include "core/renderer.h"

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

    GLFWwindow *window;
    int fullscreen;

    Renderer *renderer;

    Mesh *meshes;
    int n_meshes;

} Game;

Game* mcg_CreateGame();
void mcg_GameLoop(Game* game);
void mcg_ClearGame(Game* game);

#endif