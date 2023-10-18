//
// Created by wysoc on 10/17/2023.
//

#ifndef SPACESTATIONTOURISM_TEXTURE_H
#define SPACESTATIONTOURISM_TEXTURE_H

#include "SDL_opengl.h"

#include <string>

class NewTexture {};

class Texture {
public:
    void load(const std::string &name);
    Texture();
public:
    std::string filename;
    GLuint TextureHandle;
    int width, height, nrChannels;
};


#endif //SPACESTATIONTOURISM_TEXTURE_H
