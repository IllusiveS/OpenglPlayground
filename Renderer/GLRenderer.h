//
// Created by wysoc on 10/16/2023.
//

#ifndef SPACESTATIONTOURISM_GLRENDERER_H
#define SPACESTATIONTOURISM_GLRENDERER_H


#include <thread>
#include <mutex>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

class GLRenderer {
public:
    void StartRenderThread();
    void TriggerRender();

    static GLRenderer* Get();

    void WaitForRenderEnd();

private:
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

    void printProgramLog(GLuint program);
    void printShaderLog(GLuint shader);

    void render();

    GLuint shaderProgram;
    GLuint VBO, VAO, EBO;
    SDL_Window *Window;
    int Running{1};
};


#endif //SPACESTATIONTOURISM_GLRENDERER_H
