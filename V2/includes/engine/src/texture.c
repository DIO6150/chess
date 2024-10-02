
#include <stdlib.h>

#include "core/texture.h"

#include "glad/glad.h"

int mgeTextureObjectInit(TextureObject** _out_texture_object)
{
    *_out_texture_object = malloc(sizeof(Texture2D*));

    if (!(*_out_texture_object))
    {
        printf("%s(%s) : Texture Object couldn't be initialized.\n", __func__, __FILE__);
        return (0);
    }

    (*_out_texture_object)->object = 0;

    (*_out_texture_object)->width = 0;
    (*_out_texture_object)->height = 0;

    (*_out_texture_object)->channel_count = 0;

    glGenTextures(1, &(*_out_texture_object)->object);
    glBindTexture(GL_TEXTURE_2D_ARRAY, (*_out_texture_object)->object);

    return (1);
}

int mgeTextureLoad(const char* _in_path, Texture2D* _out_texture)
{
    /*
        TODO: load texture file, create texture, put data in texture, life good
    */
    return (1);
}

int mgeTextureCreate(Texture2D** _out_texture)
{
    *_out_texture = malloc(sizeof(Texture2D*));

    if (!(*_out_texture))
    {
        printf("%s(%s) : Texture couldn't be initialized.\n", __func__, __FILE__);
        return (0);
    }

    if (!mgeTextureObjectInit(&(*_out_texture)->object))
    {
        free(*_out_texture);
        printf("%s(%s) : Failed to initialize texture object, aborting.\n", __func__, __FILE__);
        return (0);
    }

    (*_out_texture)->data = NULL;
    
    (*_out_texture)->offset[0] = 0.0f;
    (*_out_texture)->offset[1] = 0.0f;

    return (1);
}

int mgeTextureFree(Texture2D* _in_texture)
{
    if (!_in_texture)
    {
        printf("%s(%s) : Texture is NULL.\n", __func__, __FILE__);
        return (0);
    }

    if (!_in_texture->object)
    {
        free(_in_texture);
        printf("%s(%s) : Texture Object is NULL.\n", __func__, __FILE__);
        return (0);
    }

    free(_in_texture->object);
    free(_in_texture);
    return (1);
}

int mgeTextureArray2DCreate(TextureObject** _out_texture, Texture2D* _in_textures, int _in_texture_count)
{
    if (mgeTextureObjectInit(_out_texture))
    {
        printf("%s(%s) : Failed to initialize tesxture object, aborting.\n", __func__, __FILE__);
        return (0);
    }

    int width, height, layerCount;

    // Calculate atlas size and mesh texture offset

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, layerCount);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // TODO: implements mipmap too
    for (int i = 0; i < _in_texture_count; i++)
    {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, _in_textures[i].offset[0], _in_textures[i].offset[1], 0, _in_textures[i].object->width, _in_textures[i].object->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _in_textures[i].data);
    }

    return (1);
}