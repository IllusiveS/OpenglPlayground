//
// Created by wysoc on 10/29/2023.
//

#ifndef SPACESTATIONTOURISM_IMGUIMODULE_H
#define SPACESTATIONTOURISM_IMGUIMODULE_H

#include <flecs.h>
namespace ImGUI {
    struct module {
        module(flecs::world& world); // Ctor that loads the module
    };
}


#endif //SPACESTATIONTOURISM_IMGUIMODULE_H
