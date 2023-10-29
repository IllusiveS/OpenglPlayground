//
// Created by wysoc on 10/29/2023.
//

#include "Transform.h"

void TransformModule::SetupModule(flecs::world &ecs) {
    ecs.module<TransformModule>();

    ecs.component<Transform>();

    ecs.observer<Transform>()
        .event(flecs::OnAdd)
        .each([](flecs::iter& it, size_t i, Transform& p) {
            auto CurrentEntity = it.entity(i);

            CurrentEntity.add<Transform, Local>();
            CurrentEntity.add<Transform, World>();
        });

    ecs.observer<Transform>()
        .event(flecs::OnSet)
        .each([](flecs::iter& it, size_t i, Transform& p) {
            auto CurrentEntity = it.entity(i);

            //TODO update tree values
        });
}
