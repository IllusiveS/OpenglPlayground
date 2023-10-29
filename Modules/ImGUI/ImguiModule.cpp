//
// Created by wysoc on 10/29/2023.
//

#include "ImguiModule.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "../../Renderer/GLRenderer.h"

ImGUI::module::module(flecs::world &ecs) {
    ecs.module<module>();

    ecs.system("InitImgui")
            .kind(flecs::OnStart)
            .iter([](flecs::iter &it) {
                // Setup Dear ImGui context
                IMGUI_CHECKVERSION();
                ImGui::CreateContext();
                ImGuiIO& io = ImGui::GetIO();
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
                io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
                //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

                // Setup Platform/Renderer backends
                ImGui_ImplSDL3_InitForOpenGL(GLRenderer::Get()->Window, GLRenderer::Get()->gContext);
                ImGui_ImplOpenGL3_Init();
            });

    ecs.system("BeginImGUIFrame")
        .kind(flecs::PreStore)
        .iter([](flecs::iter &it) {
            // (Where your code calls SDL_PollEvent())
            //TODO this poll event should be done in a different way, perhaps entities and components?
            SDL_Event Event;
            while (SDL_PollEvent(&Event))
            {
                ImGui_ImplSDL3_ProcessEvent(&Event); // Forward your event to backend
            }

            // (After event loop)
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL3_NewFrame();
            ImGui::NewFrame();
            ImGui::ShowDemoWindow(); // Show demo window! :)
        });

    ecs.system("EndImGUIFrame")
        .kind(flecs::PostFrame)
        .iter([](flecs::iter &it) {
            // Rendering
            // (Your code clears your framebuffer, renders your other stuff etc.)
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            // (Your code calls SDL_GL_SwapWindow() etc.)
        });
}
