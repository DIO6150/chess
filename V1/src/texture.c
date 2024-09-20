#include <stdio.h>

#include "core/texture.h"

#include "vendor/glad/glad.h"
#include "vendor/GLFW/glfw3.h"
#include "vendor/stb_image/stb_image.h"


Texture mcg_LoadTexture(const char *path)
{
    Texture texture;

    stbi_set_flip_vertically_on_load(0);

    texture.data = stbi_load(path, &texture.width, &texture.height, &texture.nb_channels, 4);

    if (texture.data == NULL) {
        printf("%s\n", stbi_failure_reason());
    }

    glGenTextures(1, &texture.object);
	glBindTexture(GL_TEXTURE_2D, texture.object);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

void mcg_FreeTexture(Texture *tex)
{
    stbi_image_free(tex->data);
}