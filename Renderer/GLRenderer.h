//
// Created by wysoc on 10/16/2023.
//

#ifndef SPACESTATIONTOURISM_GLRENDERER_H
#define SPACESTATIONTOURISM_GLRENDERER_H


#include <thread>
#include <mutex>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <map>
#include "Texture.h"
#include "flecs.h"
#include "Shader.h"

class GLRenderer;

struct RendererCommand {
    virtual void Execute(GLRenderer* renderer) = 0;
};

struct LoadMesh : public RendererCommand {

};

struct LoadTexture : public RendererCommand {

};

struct LoadShader : public RendererCommand {

};

class GLRenderer {
    friend RendererCommand;

public:
    void StartRenderThread();
    void TriggerRender();

    flecs::entity GetTexture(const std::string &filename);
    flecs::entity GetShader(const std::string &filename);

    static GLRenderer* Get();

    void WaitForRenderEnd();

private:
    flecs::world render_ecs;

    std::map<std::string, std::shared_ptr<Texture>> Textures;

    bool ShouldRender {false};
    bool IsRenderingFinished {false};

    static GLRenderer* Singleton;

    void init();
    bool initGL();

    std::thread RenderThread;
    std::mutex RenderThreadLock;
    std::condition_variable cv;

    void RenderThreadFunction();

    SDL_GLContext gContext;
    bool success;
    GLuint gProgramID;
    GLint gVertexPos2DLocation;
    GLuint gVBO;
    GLuint gIBO;

    GLuint DefaultFragShader;
    GLuint DefaultVertShader;

    void printProgramLog(GLuint program);
    void printShaderLog(GLuint shader);

    void render();

    GLuint shaderProgram;
    GLuint VBO, VAO, EBO;
    SDL_Window *Window;
    int Running{1};

    flecs::query<Texture> texture_query;
    flecs::query<Shader> shader_query;
};


#endif //SPACESTATIONTOURISM_GLRENDERER_H
