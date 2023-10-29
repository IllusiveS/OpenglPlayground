//
// Created by wysoc on 10/29/2023.
//

#ifndef SPACESTATIONTOURISM_TRANSFORM_H
#define SPACESTATIONTOURISM_TRANSFORM_H

#include "flecs/flecs.h"
#include "glm/glm.hpp"

struct Local{};
struct World{};

class TransformModule {
    static void SetupModule(flecs::world& ecs);
};

struct Transform {
    glm::mat4 LocalPos{1.0};
    glm::mat4 GlobalPos{1.0};
};


#endif //SPACESTATIONTOURISM_TRANSFORM_H
