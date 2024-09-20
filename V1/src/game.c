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
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "falling.table.chess.0.0");

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

    game->window = glfwCreateWindow(game->window_width, game->window_height, "Chess", monitor, NULL);
    glfwSetWindowAttrib(game->window, GLFW_RESIZABLE, GLFW_FALSE);

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

    game->selected[0] = (Position){-1, -1};
    game->selected[1] = (Position){-1, -1};
    game->select_index = 1;

    game->piece_position = NULL;

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

        game->window_width = 900;
        game->window_height = 900;
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

        game->board_mesh = malloc(sizeof(Mesh) * 1);
        game->pieces_mesh = malloc(sizeof(Mesh) * 32);
        game->piece_position = malloc(sizeof(Position) * 32);

        if (game->pieces_mesh == NULL)
        {
            exit(EXIT_FAILURE);
        }

        *game->board_mesh = mcg_CreateBoard2D();

        glm_translate(game->board_mesh->model_matrix, (vec3){ -8.0f, -8.0f, 0.0f });
        glm_mat4_scale(game->board_mesh->model_matrix, 0.1);

        vec4 translation;
        mat4 rot;
        vec3 sca;
        glm_decompose(game->board_mesh->model_matrix, translation, rot, sca);

        game->board_offset_x = mcg_NormalizedRange1f(translation[0]) * game->window_width;
        game->board_offset_y = mcg_NormalizedRange1f(translation[1]) * game->window_height;

        game->board_tile_width = 0.1 * game->window_width;
        game->board_tile_height = 0.1 * game->window_height;

        float offset_x, offset_y;
        float offset_dx, offset_dy;

        int k = 0;

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                Type type = mcg_GetPiece(game->board, i, j).type;
                Color color = mcg_GetPiece(game->board, i, j).color;

                if (type == CHESS_TYPE_EMPTY)
                    continue;

                game->piece_position[k] = (Position) { i, j };

                game->pieces_mesh[k] = mcg_CreatePiece2D(mcg_GetPiece(game->board, i, j).type,
                                                                 mcg_GetPiece(game->board, i, j).color);

                glm_translate(game->pieces_mesh[k].model_matrix, (vec3){ i * 2, 14.0f - j * 2, 0.0f });

                mcg_AddTexture(game->pieces_mesh + i, game->textures + 0);

                offset_x = (float)(type * 128.0f) / (float)game->textures[0].width;
                offset_y = (float)(color * 128.0f) / (float)game->textures[0].height;

                offset_dx = offset_x + (float)(128.0f / game->textures[0].width);
                offset_dy = offset_y + (float)(128.0f / game->textures[0].height);

                game->pieces_mesh[k].vertices[0].u = offset_x;
                game->pieces_mesh[k].vertices[0].v = offset_y;
                game->pieces_mesh[k].vertices[1].u = offset_dx;
                game->pieces_mesh[k].vertices[1].v = offset_y;
                game->pieces_mesh[k].vertices[2].u = offset_dx;
                game->pieces_mesh[k].vertices[2].v = offset_dy;
                game->pieces_mesh[k].vertices[3].u = offset_x;
                game->pieces_mesh[k].vertices[3].v = offset_dy;

                k++;
            }
        }

        for (int i = 0; i < 32; i++)
        {
            glm_translate(game->pieces_mesh[i].model_matrix, (vec3){ -8.0f, -8.0f, 0.0f });
            glm_mat4_scale(game->pieces_mesh[i].model_matrix, 0.1);
        }

    }

    return game;
}

static void game_loop_type_1(Game *game)
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

static void game_2D_update(Game *game)
{
    double mouse_pos_x;
    double mouse_pos_y;
    int focus;

    int mouse_tile_pos_x = -1;
    int mouse_tile_pos_y = -1;

    static int pressed = 0;


    glfwGetCursorPos(game->window, &mouse_pos_x, &mouse_pos_y);
    focus = glfwGetWindowAttrib(game->window, GLFW_FOCUSED);
    
    int mouse_button_left;

    if (focus)
    {
        int x_status = mouse_pos_x >= game->board_offset_x && mouse_pos_x <= game->board_offset_x + game->board_tile_width * 8;
        int y_status = mouse_pos_y >= game->board_offset_y && mouse_pos_y <= game->board_offset_y + game->board_tile_height * 8;

        if (x_status && y_status)
        {
            
            mouse_button_left = glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_LEFT);

            if (mouse_button_left && !pressed)
            {
                mouse_tile_pos_x = (int)(mouse_pos_x - (double)game->board_offset_x) / game->board_tile_width;
                mouse_tile_pos_y = (int)(mouse_pos_y - (double)game->board_offset_y) / game->board_tile_height;

                pressed = 1;

                if (mcg_GetPiece(game->board, mouse_tile_pos_x, mouse_tile_pos_y).type == CHESS_TYPE_EMPTY && game->select_index == 1)
                    return;

                printf("Mouse Tile Pos : %d %d\n", mouse_tile_pos_x, mouse_tile_pos_y);
                game->select_index = !game->select_index;
                game->selected[game->select_index] = (Position) {mouse_tile_pos_x, mouse_tile_pos_y};


                if (game->select_index == 1)
                {
                    if ( mcg_PieceMovement(game->board, game->selected[0], game->selected[1]) )
                    {
                        Piece eaten = mcg_MovePiece(game->board, game->selected[0].x, game->selected[0].y, game->selected[1].x, game->selected[1].y);

                        int index_a = -1;

                        for (int i = 0; i < 32; i++)
                        {
                            if (game->pieces_mesh[i].visible && game->piece_position[i].x == game->selected[0].x && game->piece_position[i].y == game->selected[0].y)
                            {
                                index_a = i;
                            }

                            if (eaten.type != CHESS_TYPE_EMPTY)
                            {
                                if (game->piece_position[i].x == game->selected[1].x && game->piece_position[i].y == game->selected[1].y)
                                {
                                    game->pieces_mesh[i].visible = 0;
                                }
                            }
                        }

                        if (index_a == -1)
                        {
                            printf("Wtf ????\n");
                            return;
                        }

                        //vec3 new_pos = {    mcg_SymetrizeRange1f((float)((float)game->selected[1].x * game->board_tile_width + game->board_offset_x) / (float)game->window_width) * 0.8f,
                          //                  mcg_SymetrizeRange1f((float)((float)game->selected[1].y * game->board_tile_height + game->board_offset_y) / (float)game->window_height) * 0.8f,
                            //                0.0f
                              //          };

                        int diff_x = game->selected[0].x - game->selected[1].x;
                        int diff_y = game->selected[0].y - game->selected[1].y;

                        vec3 new_pos = { diff_x * -2.0f, diff_y * 2.0f, 0.0f};

                        glm_translate(game->pieces_mesh[index_a].model_matrix, new_pos);
                        game->piece_position[index_a] = game->selected[1];

                        game->current_turn = !game->current_turn;

                        mcg_PrintBoard(game->board);
                    }
                }


            }
            else if (!mouse_button_left && pressed)
            {
                //printf("Mouse released\n");
                pressed = 0;
            }

        }
    }
}

void mcg_GameLoop(Game* game)
{
    if (game->type == GAMETYPE_CONSOLE)
    {
        game_loop_type_1(game);
    }
    else if (game->type == GAMETYPE_2D)
    {
        while (!glfwWindowShouldClose(game->window))
        {
            glfwPollEvents();

            game_2D_update(game);

            if (glfwGetKey(game->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                game->running = 0;
                glfwSetWindowShouldClose(game->window, GLFW_TRUE);
            }

            mcg_PushModel(game->renderer, game->board_mesh, POOL_0);

            for (int i = 0; i < 32; i++)
            {
                if (game->pieces_mesh[i].visible)
                    mcg_PushModel(game->renderer, game->pieces_mesh + i, POOL_1);
            }

            glClear(GL_COLOR_BUFFER_BIT);

            mcg_Render(game->renderer);

            glfwSwapBuffers(game->window);
        }
    }
}

void mcg_ClearGame(Game* game)
{
    mcg_ClearBoard(game->board);

    for (int i = 0; i < 32; i++)
    {
        mcg_FreeMesh(game->pieces_mesh + i);
    }
    free(game->pieces_mesh);

    mcg_FreeMesh(game->board_mesh);
    free(game->board_mesh);

    for (int i = 0; i < game->n_textures; i++)
    {
        mcg_FreeTexture(game->textures + i);
    }
    free(game->textures);

    mcg_FreeRenderer(game->renderer);

    glfwDestroyWindow(game->window);
    glfwTerminate();
    free(game);
}