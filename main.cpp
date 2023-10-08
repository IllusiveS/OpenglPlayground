#include <iostream>
#include <string>

#include "raylib.h"
#include "flecs.h"
#include "raymath.h"


struct IsOn {

};

struct Node {
    int x, y;
};

struct Cam {
    Vector2 pos{};
    Camera2D camera{};
    float desiredZoom = 1.0f;
};

struct NeighbourNode {

};

struct Player {

};

struct Wall {

};

struct Space {

};

struct Floor {

};

struct Renderable {
    Color color{RED};
};

int main() {
    std::cout << "Hello, World!" << std::endl;

    flecs::world ecs;
    ecs.set<flecs::Rest>({});
    ecs.import<flecs::monitor>();

    flecs::log::set_level(2);

    auto player = ecs.entity();
    player.add<Player>();

    ecs.system("StartupNodes")
        .kind(flecs::OnStart)
        .read<Node>()
        .write<Node>()
        .iter([](flecs::iter& it) {

            for (int x = -5; x < 5; x++) {
                for (int y = -5; y < 5; y++) {
                    std::string NodeName = "Node:" + std::to_string(x) + ":" + std::to_string(y);
                    auto space = it.world().entity(NodeName.c_str());

                    space.set<Node>({x, y});
                    //TODO add in another place
                    //space.add<Renderable>();
                    //space.add<Space>();
                }
            }

//            for (int x = 0; x < 10; x++) {
//                auto wall = it.world().entity();
//
//                wall.set<Node>({x, 0});
//                wall.add<Wall>();
//            }
    });

    ecs.system<Node>("StartupNodesNeighbourhoods")
        .kind(flecs::OnStart)
        .read<Node>()
        .multi_threaded()
        .iter([](flecs::iter& it, Node* node) {
            for(auto i : it) {
                const auto &NodeComp = node[i];
                const auto X = NodeComp.x;
                const auto Y = NodeComp.y;

                const Vector2 neighs[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

                for(auto &N : neighs) {
                    auto found_neighbour = it.world()
                        .filter<Node>()
                        .find([&](Node& FoundNode) {
                            return FoundNode.x == (X + N.x) && FoundNode.y == (Y + N.y);
                        });

                        if (found_neighbour) {
                            it.entity(i).add<NeighbourNode>(found_neighbour);
                        }
                }
            }
        });

    ecs.system<Node>("SetupSpaceTiles")
        .kind(flecs::OnStart)
        .multi_threaded()
        .iter([](flecs::iter &it, Node* nodes) {
            for(auto i : it) {
                auto space = it.world().entity();

                space.add<Renderable>();
                space.add<Space>();
                space.add<IsOn>(it.entity(i));
            }
        });

    ecs.system("StartupRaylib")
        .kind(flecs::OnStart)
        .iter([](flecs::iter& it) {
            // Initialization
            //--------------------------------------------------------------------------------------
            const int screenWidth = 1920;
            const int screenHeight = 1080;

            InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

            SetTraceLogLevel(LOG_NONE);

            it.world().set<Cam>({0, 0});
            auto cam = it.world().get_mut<Cam>();
            cam->camera.target = {0, 0};
            cam->camera.offset = {screenWidth/2.0f, screenHeight/2.0f};
            cam->camera.rotation = 0.0f;
            cam->camera.zoom = 1.0f;

            SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
        });

    ecs.system<Player>("Player")
            .kind(flecs::OnUpdate)
            .iter([&](flecs::iter& it, Player* r) {
                auto *cam = it.world().get_mut<Cam>();
                auto &camera = cam->camera;

                // Camera zoom controls
                cam->desiredZoom += ((float)GetMouseWheelMove() * 2.75f * it.delta_time());

                if (cam->desiredZoom > 3.0f) cam->desiredZoom = 3.0f;
                else if (cam->desiredZoom < 0.1f) cam->desiredZoom = 0.1f;

                cam->camera.zoom = Lerp(cam->camera.zoom, cam->desiredZoom, it.delta_time() * 10);

                const auto CamSpeed = 230 * it.delta_time() / cam->camera.zoom;

                if (IsKeyDown(KEY_RIGHT)) camera.target.x += CamSpeed;
                else if (IsKeyDown(KEY_LEFT)) camera.target.x -= CamSpeed;
                if (IsKeyDown(KEY_DOWN)) camera.target.y += CamSpeed;
                else if (IsKeyDown(KEY_UP)) camera.target.y -= CamSpeed;

            });

    ecs.system<Player>("BeginRaylibRender")
            .kind(flecs::PostUpdate)
            .write<Player>()
            .iter([](flecs::iter& it) {
                BeginDrawing();
                BeginMode2D(it.world().get<Cam>()->camera);
                ClearBackground(RAYWHITE);
            });

    auto RenderRule = ecs.rule_builder()
            .with<IsOn>("$PotentialNode")
            .with<Node>().src("$PotentialNode")
            .instanced()
            .build();


    int NodeVar = RenderRule.find_var("PotentialNode");

    ecs.system<Renderable, IsOn>("Render")
            .kind(flecs::PostUpdate)
            .read<Player>()
            .instanced()
            .term_at(2).second(flecs::Wildcard)
            .iter([&](flecs::iter& it) {
                RenderRule.each([&](flecs::iter& itr, size_t index) {
                    auto Entity = itr.entity(index);
                    auto NodeComp = itr.get_var(NodeVar).get<Node>();

                    if (NodeComp) {
                        int X = NodeComp->x * 90;
                        int Y = NodeComp->y * 90;
                        DrawRectangle( X, Y, 80, 80, RED);
                    }
                });
            });

    ecs.system<Player>("EndRaylibRender")
            .kind(flecs::PostUpdate)
            .term<Player>()
            .write<Player>()
            .iter([](flecs::iter& it) {
                EndMode2D();
                EndDrawing();
            });
    //--------------------------------------------------------------------------------------


    ecs.progress(0);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        ecs.progress(GetFrameTime());
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    //CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;

    return 0;
}
