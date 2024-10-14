
#include <stdlib.h>
#include <stdio.h>

#include "core/texture.h"

#include "glad/glad.h"

int mgeTextureObjectInit(TextureObject** _out_texture_object)
{
    *_out_texture_object = malloc(sizeof(TextureObject));

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

int mgeTextureObjectFree(TextureObject* _in_texture_object)
{
    if (!_in_texture_object)
    {
        printf("%s(%s) : Texture Object is NULL.\n", __func__, __FILE__);
        return (0);
    }

    free(_in_texture_object);

    return (1);
}

int mgeTextureLoad(__attribute__((unused))const char* _in_path, __attribute__((unused))Texture2D* _out_texture)
{
    /*
        TODO: load texture file, create texture, put data in texture, life good
    */
    return (1);
}

int mgeTextureCreate(Texture2D** _out_texture)
{
    *_out_texture = malloc(sizeof(Texture2D));

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
        printf("%s(%s) : Failed to initialize texture object, aborting.\n", __func__, __FILE__);
        return (0);
    }

    int width, height;

    // Calculate atlas size and mesh texture offset
    //      * We will do something really dumb here: 
    //      * What if... ? We disguised fast food as our own cooking ? Ohohoh delightfully devilish Seymour
    //      * Nah kidding, we just queue the textures horizontally so height is always the max texture height :/
    width = 0;
    height = _in_textures[0].object->height;

    for (int i = 0; i < _in_texture_count; i++)
    {
        width += _in_textures[i].object->width;
        if (height < _in_textures[i].object->height) height = _in_textures[i].object->height;
    }

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, width, height, 1);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    float offset_x = 0.0f;
    float offset_y = 0.0f;

    // TODO: implements mipmap too
    for (int i = 0; i < _in_texture_count; i++)
    {
        _in_textures[i].offset[0] = offset_x / width;
        _in_textures[i].offset[1] = offset_y / height;

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, _in_textures[i].offset[0], _in_textures[i].offset[1], 0, _in_textures[i].object->width, _in_textures[i].object->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _in_textures[i].data);

        offset_x += _in_textures[i].object->width;
        offset_y += _in_textures[i].object->height;
    }

    return (1);
}