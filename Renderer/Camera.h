//
// Created by wysoc on 10/18/2023.
//

#ifndef SPACESTATIONTOURISM_CAMERA_H
#define SPACESTATIONTOURISM_CAMERA_H


#include "glm/vec3.hpp"
#include "glm/ext/matrix_float4x4.hpp"

class Camera {
public:
    Camera();
public:
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraDirection;
    glm::vec3 cameraUp;
    glm::vec3 cameraRight;
    glm::mat4 view;
    glm::mat4 projection;
};


#endif //SPACESTATIONTOURISM_CAMERA_H
