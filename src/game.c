#include "core/utils.h"

#include "game/board.h"
#include "game/game.h"
#include "game/piece_check.h"
#include "game/graphics/board2D.h"
#include "game/graphics/piece2D.h"

#include <stdio.h>
#include <stdlib.h>

// thanks to OutofBound answer in https://stackoverflow.com/questions/60486285/gldebugmessagecallback-doesnt-get-called-despite-of-error
static char* error_description(GLenum err)
{
    switch (err) {
    case GL_NO_ERROR:
        return "GL_NO_ERROR: No error has been recorded. The value of this symbolic constant is guaranteed to be 0. ";
    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.  ";
    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.  ";
    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.  ";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return
            "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete."
            "The offending command is ignored and has no other side effect than to set the error flag.";
    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded. . ";
    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW: An attempt has been made to perform an operation that would cause an internal stack to underflow. ";
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW: An attempt has been made to perform an operation that would cause an internal stack to overflow. ";
    default:
        return "No Description";
    }
}

void framebuffer_size_callback(__attribute__((unused))GLFWwindow* m_Window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void message_callback(__attribute__((unused))GLenum source, GLenum type, GLuint id, GLenum severity, __attribute__((unused))GLsizei length, const GLchar* message, __attribute__((unused))const void* userParam)
{
    printf("Severity : %d, Type : %d, ID : %d\n", severity, type, id);
    printf("%s\n", error_description(type));
    printf("%s\n", message);
}

static void create_render_context(Game *game)
{
    if (!glfwInit())
    {
        mcg_ClearGame(game);
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWmonitor* monitor;

    if (game->fullscreen)
    {
        int count;
        GLFWmonitor** monitors = glfwGetMonitors(&count);
        
        if (count != 0) {
            monitor = *monitors;
        }
    }
    else
    {
        monitor = NULL;
    }

    game->window = glfwCreateWindow(900, 900, "Chess", monitor, NULL);

    glfwMakeContextCurrent(game->window);

    // we load glad
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("GL %d.%d\n", GLVersion.major, GLVersion.minor);

    // size of the rendering m_Window
    glViewport(0, 0, 900, 900);

    // set the framebuffer resize callback
    glfwSetFramebufferSizeCallback(game->window, framebuffer_size_callback);

    glClearColor(0.2f, 0.5f, 0.7f, 1.0f);

    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);
    // glCullFace(GL_FRONT);
    // glFrontFace(GL_CW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(message_callback, 0);
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

Game* mcg_CreateGame()
{
    Game* game;
    game = (Game *) malloc(sizeof(Game));

    if (game == NULL)
    {
        exit(EXIT_FAILURE);
    }

    game->type = GAMETYPE_2D;
    game->board = mcg_CreateBoard();
    mcg_InitBoard(game->board);

    game->current_turn = CHESS_COLOR_WHITE;
    game->winner = CHESS_COLOR_NONE;
    game->running = 1;

    game->window = NULL;
    game->fullscreen = 0;
    game->renderer = NULL;

    /*Piece tab[8][8] = {
        { {CHESS_TYPE_ROOK, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_KNIGHT, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_BISHOP, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_QUEEN, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_BISHOP, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_KNIGHT, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_ROOK, CHESS_COLOR_BLACK, 0} },
        { {CHESS_TYPE_PAWN, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_KING, CHESS_COLOR_BLACK, 1}, {CHESS_TYPE_PAWN, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_BLACK, 0} },
        { {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0} },
        { {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_BLACK, 0}, {CHESS_TYPE_KNIGHT, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0} },
        { {CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0} },
        { {CHESS_TYPE_BISHOP, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0} },
        { {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_PAWN, CHESS_COLOR_WHITE, 0} },
        { {CHESS_TYPE_ROOK, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_KNIGHT, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_EMPTY, CHESS_COLOR_NONE, 0}, {CHESS_TYPE_QUEEN, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_KING, CHESS_COLOR_WHITE, 0}, {CHESS_TYPE_BISHOP, CHESS_COLOR_WHITE, 0}, {}, {CHESS_TYPE_ROOK, CHESS_COLOR_WHITE, 0} }
    };

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            mcg_SetPiece(game->board, tab[j][i], i, j);
        }
    }*/

    if (game->type == GAMETYPE_2D)
    {
        // RENDER CONTEXT SETUP //

        create_render_context(game);

        // GAME SETUP //

        game->renderer = mcg_CreateRenderer();

        game->textures = malloc(sizeof(Texture) * 1);

        if (game->textures == NULL)
        {
            exit(EXIT_FAILURE);
        }

        game->textures[0] = mcg_LoadTexture("assets/chess.png");
        game->n_textures++;


        game->meshes = malloc(sizeof(Mesh) * 33);

        if (game->meshes == NULL)
        {
            exit(EXIT_FAILURE);
        }

        game->meshes[0] = mcg_CreateBoard2D();
        game->n_meshes = 1;

        float offset_x, offset_y;
        float offset_dx, offset_dy;

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                Type type = mcg_GetPiece(game->board, i, j).type;
                Color color = mcg_GetPiece(game->board, i, j).color;

                if (type == CHESS_TYPE_EMPTY)
                    continue;

                game->meshes[game->n_meshes] = mcg_CreatePiece2D(mcg_GetPiece(game->board, i, j).type,
                                                                 mcg_GetPiece(game->board, i, j).color);

                glm_translate(game->meshes[game->n_meshes].model_matrix, (vec3){ 14.0f - i * 2, 14.0f - j * 2, 0.0f });

                mcg_AddTexture(game->meshes + game->n_meshes, game->textures);

                offset_x = (type * 128) / game->textures[0].width;
                offset_y = (color * 128) / game->textures[0].height;

                offset_dx = offset_x + 128;
                offset_dy = offset_y + 128;

                game->meshes[game->n_meshes].vertices.v2D[0].u = offset_x;
                game->meshes[game->n_meshes].vertices.v2D[0].v = offset_dy;

                game->meshes[game->n_meshes].vertices.v2D[1].u = offset_dx;
                game->meshes[game->n_meshes].vertices.v2D[1].v = offset_dy;

                game->meshes[game->n_meshes].vertices.v2D[2].u = offset_dx;
                game->meshes[game->n_meshes].vertices.v2D[2].v = offset_y;

                game->meshes[game->n_meshes].vertices.v2D[3].u = offset_x;
                game->meshes[game->n_meshes].vertices.v2D[3].v = offset_y;
                    
                game->n_meshes++;
            }
        }

        for (int i = 0; i < game->n_meshes; i++)
        {
            glm_translate(game->meshes[i].model_matrix, (vec3){ -8.0f, -8.0f, 0.0f });
            glm_mat4_scale(game->meshes[i].model_matrix, 0.1);
        }

    }

    return game;
}

void mcg_GameLoop(Game* game)
{

    char text[2][256] = {
        "joueur 1 (Blanc)",
        "joueur 2  (Noir)"
    };

    Piece piece_a;
    Position pos_a;

    Piece piece_b;
    Position pos_b;
    
    int castled = 0;

    if (game->type == GAMETYPE_CONSOLE)
    {
        while (game->running)
        {

            if (game->current_turn != CHESS_COLOR_NONE)
            {
                printf("--------------------------------\n");
                printf("--- Tour du %s ---\n", text[game->current_turn]);
                printf("--------------------------------\n");
                mcg_PrintBoard(game->board);

                char choice = '\0';

                printf("    -> Quitter (q)\n");
                printf("    -> Continuer (n'importe quelle touche)\n");
                scanf("%c", &choice);
                mcg_ClearInputStream();

                if (choice != 'q' && choice != 'Q')
                {
                    int x;
                    int y;
                    int dx;
                    int dy;

                    do
                    {
                        printf("    -> Ecrivez les coordonnees de départ et d'arrivee de votre piece : ( X Y DX DY )\n");
                        scanf("%d %d %d %d", &x, &y, &dx, &dy);
                        mcg_ClearInputStream();

                        if (x > 7 || x < 0 || y > 7 || y < 0 || dx > 7 || dx < 0 || dy > 7 || dy < 0)
                            continue;

                        if (x == dx && y == dy)
                        {
                            x = y = dx = dy = -8;
                            continue;
                        }
                        
                        piece_a = mcg_GetPiece(game->board, x, y);
                        pos_a = (Position) {x, y};
                        if (piece_a.color != game->current_turn)
                        {
                            x = y = dx = dy = -8;
                            continue;
                        }

                        piece_b = mcg_GetPiece(game->board, dx, dy);
                        pos_b = (Position) {dx, dy};
                        if (piece_b.color == game->current_turn)
                        {
                            x = y = dx = dy = -8;
                            continue;
                        }

                        if (piece_a.type == CHESS_TYPE_KING &&
                            piece_b.type == CHESS_TYPE_ROOK &&
                            piece_a.color == piece_b.color)
                        {
                            if (mcg_CanCastle(game->board, pos_a, pos_b))
                            {
                                mcg_Castle(game->board, pos_a, pos_b);
                                castled = 1;
                                break;
                            }
                        }
                        else if (piece_b.type == CHESS_TYPE_KING &&
                                piece_a.type == CHESS_TYPE_ROOK &&
                                piece_a.color == piece_b.color)
                        {
                            if (mcg_CanCastle(game->board, pos_b, pos_a))
                            {
                                mcg_Castle(game->board, pos_b, pos_a);
                                castled = 1;
                                break;
                            }
                        }

                    } while (!mcg_PieceMovement(game->board, (Position){x, y}, (Position){dx, dy}));

                    if (!castled)
                        mcg_MovePiece(game->board, x, y, dx, dy);
                    else
                        castled = 0;

                    // promote check
                    if (dy == 0 || dy == 7)
                    {
                        if (mcg_GetPiece(game->board, dx, dy).type == CHESS_TYPE_PAWN)
                        {
                            printf("    -> Ce pion va être promu, choisissez le rang de la pièce : \n");
                            printf("        > (Q) Reine \n");
                            printf("        > (H) Cavalier \n");
                            printf("        > (R) Tour \n");
                            printf("        > (B) Fou \n");

                            do
                            {
                                scanf("%c", &choice);
                                mcg_ClearInputStream();

                            } while (choice != 'Q' && choice != 'H' && choice != 'R' && choice != 'B');

                            if (choice == 'Q')
                                mcg_ChangePieceType(game->board, CHESS_TYPE_QUEEN, dx, dy);

                            else if (choice == 'H')
                                mcg_ChangePieceType(game->board, CHESS_TYPE_KNIGHT, dx, dy);

                            else if (choice == 'R')
                                mcg_ChangePieceType(game->board, CHESS_TYPE_ROOK, dx, dy);

                            else if (choice == 'B')
                                mcg_ChangePieceType(game->board, CHESS_TYPE_BISHOP, dx, dy);
                        }
                    }

                    // did the current player put himself in check
                    if (mcg_IsCheck(game->board, mcg_GetKing(game->board, game->current_turn)))
                    {
                        mcg_SetPiece(game->board, piece_a, pos_a.x, pos_a.y);
                        mcg_SetPiece(game->board, piece_b, pos_b.x, pos_b.y);

                        if (mcg_IsCheckmate(game->board, game->current_turn))
                        {
                            game->winner = !game->current_turn;
                        }
                        else
                        {
                            printf("    /!\\ Attention ! tu te mettrais en check !\n");
                            continue;
                        }
                    }

                    // did the current player checkmate the opponent
                    if (mcg_IsCheckmate(game->board, !game->current_turn))
                    {
                        game->winner = game->current_turn;
                    }

                    game->current_turn = !game->current_turn;
                }
                else
                    game->running = 0;
            }
            else
            {
                game->current_turn = CHESS_COLOR_WHITE;
            }

            if (game->winner != CHESS_COLOR_NONE)
            {
                printf("------------------------------------------------------\n");
                printf("--------- Le gagnant est %s ------------\n", text[game->winner]);
                printf("------------------------------------------------------\n");
                game->running = 0;
            }
        }
    }
    else if (game->type == GAMETYPE_2D)
    {
        while (game->running)
        {
            glfwPollEvents();

            if (glfwGetKey(game->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) game->running = 0;

            for (int i = 0; i < game->n_meshes; i++)
            {
                mcg_PushModel(game->renderer, game->meshes + i, 0);
            }

            glClear(GL_COLOR_BUFFER_BIT);

            mcg_Render(game->renderer);

            glfwSwapBuffers(game->window);
        }
    }
}

void mcg_ClearGame(Game* game)
{
    glfwTerminate();
    mcg_ClearBoard(game->board);

    for (int i = 0; i < game->n_meshes; i++)
    {
        mcg_FreeMesh(game->meshes + i);
    }
    free(game->meshes);

    for (int i = 0; i < game->n_textures; i++)
    {
        mcg_FreeTexture(game->textures + i);
    }
    free(game->textures);

    mcg_FreeRenderer(game->renderer);
    free(game);
}