#include <stdio.h>

#include "game/game.h"

int main(__attribute__((unused))int argc, __attribute__((unused))char **argv)
{
    Game* game;
    
    game = mcg_CreateGame();

    mcg_GameLoop(game);

    mcg_ClearGame(game);

    return (0);
}