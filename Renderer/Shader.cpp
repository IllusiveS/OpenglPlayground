//
// Created by wysoc on 10/17/2023.
//

#include "GL/glew.h"
#include "Shader.h"

#include <fstream>
#include <sstream>
#include <cassert>

Shader::Shader() {

}

Shader::~Shader() {
    glDeleteShader(shaderProgram);
}

void Shader::load() {
    auto dot_pos = filename.find_last_of('.');
    auto extension = filename.substr(dot_pos, 5);

    if (extension == ".vert") {
        type = SHADER_VERTEX;
    } else if (extension == ".frag") {
        type = SHADER_FRAGMENT;
    } else if (extension == ".comp") {
        assert(false);
    } else {
        assert(false);
    }

    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string content = buffer.str();
    const char *c_str = content.c_str();

    shaderProgram = glCreateShader(DetermineShaderType());
    glShaderSource(shaderProgram, 1, &c_str, nullptr);
    glCompileShader(shaderProgram);
}

GLuint Shader::DetermineShaderType() const {
    switch (type) {
        case SHADER_FRAGMENT:
            return GL_FRAGMENT_SHADER;
            break;
        case SHADER_VERTEX:
            return GL_VERTEX_SHADER;
            break;
        case SHADER_COMPUTE:
            return GL_COMPUTE_SHADER;
            break;
    }
    assert(false);
    return 0;
}
