#ifndef H_MGE_TEXTURE
#define H_MGE_TEXTURE

typedef struct
{
    unsigned int object;

    int width;
    int height;
    int channel_count;
} TextureObject;

typedef struct
{
    TextureObject* object;
    unsigned char* data;

    float offset[2];
} Texture2D;

int mgeTextureObjectInit(TextureObject** _out_texture_object);
int mgeTextureObjectFree(TextureObject* _in_texture_object);

int mgeTextureCreate(Texture2D** _out_texture);
int mgeTextureFree(Texture2D* _in_texture);

int mgeTextureLoad(const char* _in_path, Texture2D* _out_texture);

int mgeTextureArray2DCreate(TextureObject** _out_texture, Texture2D* _in_textures, int _in_texture_count);


#endif