#ifndef H_MGE_TEXTURE
#define H_MGE_TEXTURE

typedef struct
{
    unsigned char* data;
    int width;
    int height;
    int channel_count;

    unsigned int opengl_object;
} Texture;

int mgeTextureLoad(const char* _in_path, Texture* _out_texture);
int mgeTextureFree(Texture* _in_texture);

#endif