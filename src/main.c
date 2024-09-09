#include <stdio.h>

#include "game/game.h"

#define _CRTDBG_MAP_ALLOC
#include "crtdbg.h"

int main(__attribute__((unused))int argc, char **argv)
{
    printf("%s\n", argv[0]);

    Game* game;
    
    game = mcg_CreateGame();

    mcg_GameLoop(game);

    mcg_ClearGame(game);

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 
    _CrtDumpMemoryLeaks();

    return (0);
}