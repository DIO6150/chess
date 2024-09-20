#ifndef H_CHESS_TEXTURE
#define H_CHESS_TEXTURE

typedef struct
{
    unsigned char* data;
    int width, height;
    int nb_channels;

    unsigned int object;
} Texture;

Texture mcg_LoadTexture(const char *path);
void mcg_FreeTexture(Texture *tex);

#endif