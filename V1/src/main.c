#include <stdio.h>

#include "game/game.h"

int main(__attribute__((unused))int argc, char **argv)
{
    printf("%s\n", argv[0]);

    Game* game;
    
    game = mcg_CreateGame();

    mcg_GameLoop(game);

    mcg_ClearGame(game);

    return (0);
}
