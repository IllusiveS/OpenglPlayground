//
// Created by wysoc on 10/17/2023.
//

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "GL/glew.h"
#include "Texture.h"

void Texture::load(const std::string &name) {
    unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0);

    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
}

Texture::Texture()
    : filename(), TextureHandle(), width(-1), height(-1), nrChannels(-1)
{

}
