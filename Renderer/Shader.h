//
// Created by wysoc on 10/17/2023.
//

#ifndef SPACESTATIONTOURISM_SHADER_H
#define SPACESTATIONTOURISM_SHADER_H

#include "SDL_opengl.h"

#include <string>

enum ShaderType {
    SHADER_VERTEX
    ,SHADER_FRAGMENT
    ,SHADER_COMPUTE
};

class NewShader {};

class Shader {
    GLuint shaderProgram;
public:
    std::string filename;
public:
    Shader();
    ~Shader();
    void load();

    ShaderType GetShaderType() const {return type;};
private:
    ShaderType type;

    GLuint DetermineShaderType() const;
};


#endif //SPACESTATIONTOURISM_SHADER_H
