#include <iostream>
#include <string>
#include <thread>
#include <queue>
#include <map>
#include <set>
#include <unordered_set>

//#include "raylib.h"
#include "flecs.h"
//#include "raymath.h"

#include "Renderer/GLRenderer.h"

#include "third-party/tracy/public/tracy/Tracy.hpp"

struct Hash {
    size_t operator() (const flecs::entity &interval) const {
        return (interval.id());
    }
};

constexpr int MapSize = 70;

struct IsOn {

};

struct Node {
    int x = 0;
    int y = 0;
    std::vector<flecs::entity> Neighbours;
};

struct DirtyPathfinding {

};

struct Cam {
    //Vector2 pos{};
    //Camera2D camera{};
    //float desiredZoom = 1.0f;
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
    //Color color{RED};
};

int main() {
    GLRenderer::Get()->StartRenderThread();
    do {
        GLRenderer::Get()->TriggerRender();
        using namespace std::chrono_literals;
        const auto start = std::chrono::high_resolution_clock::now();
        FrameMark;
        ZoneScopedN("Main");
        GLRenderer::Get()->WaitForRenderEnd();
        const auto end = std::chrono::high_resolution_clock::now();
        const auto FrameTime = 1000ms / 60;

        std::this_thread::sleep_for((FrameTime) - (end - start));
    } while(true);

    return 0;
//    std::cout << "Hello, World!" << std::endl;
//
//    flecs::world ecs;
//
//    ecs.set_threads(std::thread::hardware_concurrency());
//
//    ecs.set<flecs::Rest>({});
//    ecs.import<flecs::monitor>();
//
//    flecs::log::set_level(0);
//
//    auto player = ecs.entity();
//    player.add<Player>();
//
//
//
//    ecs.system("StartupNodes")
//        .kind(flecs::OnStart)
//        .read<Node>()
//        .multi_threaded()
//        .each([&](flecs::iter& it, size_t index) {
//
//            for (int x = -MapSize; x < MapSize; x++) {
//                for (int y = -MapSize; y < MapSize; y++) {
//                    std::string NodeName = "Node:" + std::to_string(x) + ":" + std::to_string(y);
//                    auto space = it.world().entity(NodeName.c_str());
//
//                    space.set<Node>({x, y});
//                    space.add<DirtyPathfinding>();
//                }
//            }
//    });
//
//    ecs.system<Node>("StartupNodesNeighbourhoods")
//        .kind(flecs::OnStart)
//        .read<Node>()
//        .multi_threaded()
//        .each([](flecs::iter& it, size_t index, Node& node) {
//            ZoneScopedN("StartupNodesNeighbourhoods");
//            const auto &NodeComp = node;
//            const auto X = NodeComp.x;
//            const auto Y = NodeComp.y;
//
//            const Vector2 neighs[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
//
//            for(auto &N : neighs) {
//                auto found_neighbour = it.world()
//                        .filter<Node>()
//                        .find([&](Node& FoundNode) {
//                            return FoundNode.x == (X + N.x) && FoundNode.y == (Y + N.y);
//                        });
//
//                if (found_neighbour) {
//                    node.Neighbours.push_back(found_neighbour);
//                }
//            }
//        });
//
//    ecs.system<Node>("SetupSpaceTiles")
//        .kind(flecs::OnStart)
//        .multi_threaded()
//        .read<Node>()
//        .each([](flecs::iter &it, size_t index, Node& nodes) {
//            ZoneScopedN("SetupSpaceTiles");
//            auto space = it.world().entity();
//
//            space.add<Renderable>();
//            space.add<Space>();
//            space.add<IsOn>(it.entity(index));
//        });
//
//    ecs.system("StartupRaylib")
//        .kind(flecs::OnStart)
//        .iter([](flecs::iter& it) {
//            ZoneScopedN("StartupRaylib");
//            // Initialization
//            //--------------------------------------------------------------------------------------
//            const int screenWidth = 1920;
//            const int screenHeight = 1080;
//            SetTraceLogLevel(LOG_NONE);
//
//            InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
//
//
//            it.world().set<Cam>({0, 0});
//            auto cam = it.world().get_mut<Cam>();
//            cam->camera.target = {0, 0};
//            cam->camera.offset = {screenWidth/2.0f, screenHeight/2.0f};
//            cam->camera.rotation = 0.0f;
//            cam->camera.zoom = 1.0f;
//
//            SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
//        });
//
//    ecs.system<Player>("Player")
//            .kind(flecs::OnUpdate)
//            .iter([&](flecs::iter& it, Player* r) {
//                ZoneScopedN("Player");
//                auto *cam = it.world().get_mut<Cam>();
//                auto &camera = cam->camera;
//
//                // Camera zoom controls
//                cam->desiredZoom += ((float)GetMouseWheelMove() * 2.75f * it.delta_time());
//
//                if (cam->desiredZoom > 3.0f) cam->desiredZoom = 3.0f;
//                else if (cam->desiredZoom < 0.1f) cam->desiredZoom = 0.1f;
//
//                cam->camera.zoom = Lerp(cam->camera.zoom, cam->desiredZoom, it.delta_time() * 10);
//
//                const auto CamSpeed = 230 * it.delta_time() / cam->camera.zoom;
//
//                if (IsKeyDown(KEY_RIGHT)) camera.target.x += CamSpeed;
//                else if (IsKeyDown(KEY_LEFT)) camera.target.x -= CamSpeed;
//                if (IsKeyDown(KEY_DOWN)) camera.target.y += CamSpeed;
//                else if (IsKeyDown(KEY_UP)) camera.target.y -= CamSpeed;
//
//            });
//
//    ecs.system<Node, DirtyPathfinding>("Recalculate paths")
//            .kind(flecs::OnUpdate)
//            .multi_threaded()
//            .each([](flecs::iter& it, size_t index, Node& node, DirtyPathfinding& dirty) {
//                ZoneScopedN("RecalculatePath");
//                const auto &NodeComp = node;
//                const auto X = NodeComp.x;
//                const auto Y = NodeComp.y;
//
//                //static auto cmp = [](flecs::entity left, flecs::entity right) {
//                //std::cout << "New search" << std::endl;
//                //};
//                //std::priority_queue<flecs::entity, std::vector<flecs::entity>, decltype(cmp)> Unvisited(cmp);
//
//                std::map<flecs::entity, int> Costs;
//                std::map<flecs::entity, int> CameFrom;
//                //Costs[it.entity(index)] = 0;
//                std::vector<flecs::entity> UnvisitedSet;
//                UnvisitedSet.reserve((MapSize * 2) * (MapSize * 2));
//                std::set<flecs::entity> VisitedSet;
//                UnvisitedSet.push_back(it.entity(index));
//                VisitedSet.insert(it.entity(index));
//
//                do {
//                    auto current = UnvisitedSet.back();
//                    UnvisitedSet.pop_back();
//
//                    VisitedSet.insert(current);
//
//                    auto PotentialNode = current.get<Node>();
//                    assert(PotentialNode);
//
//                    for (auto Neigh : PotentialNode->Neighbours) {
//                        //const auto CurrentCost = Costs[current];
//                        //const auto NewCost = CurrentCost + 1;
//
//                        //CameFrom.insert(std::make_pair(Neigh, current));
//
//                        if (VisitedSet.find(Neigh) == std::end(VisitedSet)) {
//                            UnvisitedSet.push_back(Neigh);
//                        }
//                    }
//                } while(!UnvisitedSet.empty());
//                it.entity(index).remove<DirtyPathfinding>();
//            });
//
//    ecs.system<Player>("BeginRaylibRender")
//            .kind(flecs::PostUpdate)
//            .write<Player>()
//            .iter([](flecs::iter& it) {
//                ZoneScopedN("BeginRaylibRender");
//                BeginDrawing();
//                BeginMode2D(it.world().get<Cam>()->camera);
//                ClearBackground(RAYWHITE);
//            });
//
//    ecs.system<Renderable, IsOn>("Render")
//            .kind(flecs::PostUpdate)
//            .read<Player>()
//            .term_at(2).second(flecs::Wildcard)//.ctx(&RenderRule)
//            .iter([](flecs::iter& it) {
//                ZoneScopedN("Render");
//                for (auto i: it) {
//                    auto RenderableEnt = it.entity(i);
//                    flecs::entity renderable = it.pair(2).first();
//                    flecs::entity node = it.pair(2).second();
//                    auto NodeComp = node.get<Node>();
//
//                    if (NodeComp) {
//                        int X = NodeComp->x * 90;
//                        int Y = NodeComp->y * 90;
//                        DrawRectangle( X, Y, 80, 80, RED);
//                    }
//                }
//            });
//
//    ecs.system<Player>("EndRaylibRender")
//            .kind(flecs::PostUpdate)
//            .term<Player>()
//            .write<Player>()
//            .iter([](flecs::iter& it) {
//                ZoneScopedN("EndRaylibRender");
//                EndMode2D();
//                EndDrawing();
//            });
//    //--------------------------------------------------------------------------------------
//
//
//    ecs.progress(0);
//    // Main game loop
//    while (!WindowShouldClose())    // Detect window close button or ESC key
//    {
//        FrameMark;
//        ecs.progress(GetFrameTime());
//    }
//
//    // De-Initialization
//    //--------------------------------------------------------------------------------------
//    //CloseWindow();        // Close window and OpenGL context
//    //--------------------------------------------------------------------------------------

    return 0;

}
