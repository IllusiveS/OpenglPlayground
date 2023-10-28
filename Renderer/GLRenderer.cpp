//
// Created by wysoc on 10/16/2023.
//

#include <cassert>
#include <gl/glew.h>
#include <chrono>
#include <iostream>
#include "GLRenderer.h"
#include "tracy/Tracy.hpp"
#include "flecs/addons/cpp/mixins/query/impl.hpp"
#include "Camera.h"

GLRenderer* GLRenderer::Singleton = nullptr;

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\n\0";



void GLRenderer::StartRenderThread() {
    std::thread RenderThreadObject(&GLRenderer::RenderThreadFunction, this);
    RenderThread = std::move(RenderThreadObject);
}

void GLRenderer::init() {
    uint32_t WindowFlags = SDL_WINDOW_OPENGL;


    Window = SDL_CreateWindow("OpenGL Test", 800, 600, WindowFlags);
    assert(Window);

    int32_t Running = 1;
    int32_t FullScreen = 0;

    //Create context
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 2 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    gContext = SDL_GL_CreateContext( Window );
    if( gContext == nullptr )
    {
        printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {

        //Initialize GLEW
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if( glewError != GLEW_OK )
        {
            printf( "Error initializing GLEW! %s\n", glewGetErrorString( glewError ) );
        }

        //Use Vsync
        if( SDL_GL_SetSwapInterval( 1 ) < 0 )
        {
            printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
        }

        //Initialize OpenGL
        if( !initGL() )
        {
            printf( "Unable to initialize OpenGL!\n" );
            success = false;
        }
    }

    texture_query = render_ecs.query_builder<Texture>().build();
    shader_query = render_ecs.query_builder<Shader>().build();
    model_query = render_ecs.query_builder<Model>().build();

    auto FragShader = GetShader("shaders/fragment.frag");
    auto VertShader = GetShader("shaders/vertex.vert");

    ProgramEntity = render_ecs.entity("DefaultProgram");
    ProgramEntity.add<HasFragmentShader>(FragShader);
    ProgramEntity.add<HasVertexShader>(VertShader);
    ProgramEntity.add<ShaderProgram>();
    ProgramEntity.add<NewShaderProgram>();

    GetModel("meshes/cube.obj");

    auto CameraEnt = render_ecs.entity("Camera");
    CameraEnt.add<Camera>();

    render_ecs.system<Model, NewModel>("Load Models")
            .iter([](flecs::iter it, Model* tex, NewModel* new_tex){
                ZoneScopedN("Load Meshes");
                for (auto i : it) {
                    auto &current_tex = tex[i];
                    current_tex.loadModel(current_tex.filename);
                    it.entity(i).remove<NewMesh>();
                }
            });

//    render_ecs.system<Mesh, NewMesh>("Load Meshes")
//            .iter([](flecs::iter it, Mesh* tex, NewMesh* new_tex){
//                ZoneScopedN("Load Meshes");
//                for (auto i : it) {
//                    auto &current_tex = tex[i];
//                    current_tex.load();
//                    it.entity(i).remove<NewMesh>();
//                }
//            });

    render_ecs.system<Shader, NewShader>("Load Shaders")
            .iter([](flecs::iter it, Shader* tex, NewShader* new_tex){
                ZoneScopedN("Load Shaders");
                for (auto i : it) {
                    auto &current_tex = tex[i];
                    current_tex.load();
                    it.entity(i).remove<NewShader>();
                }
            });

    render_ecs.system<ShaderProgram, NewShaderProgram>("Setup programs")
            .iter([&](flecs::iter it, ShaderProgram* Progr, NewShaderProgram* NewProgr) {
                ZoneScopedN("Load Programs");
                for (auto i : it) {
                    auto VertShaderEntity = it.entity(i).target<HasVertexShader>();
                    assert(VertShaderEntity.has<Shader>());
                    auto VertShader = VertShaderEntity.get<Shader>();
                    auto FragShaderEntity = it.entity(i).target<HasFragmentShader>();
                    assert(FragShaderEntity.has<Shader>());
                    auto FragShader = VertShaderEntity.get<Shader>();

                    auto &ShaderProg = Progr[i];
                    ShaderProg.ProgramID = glCreateProgram();
                    glAttachShader(ShaderProg.ProgramID, VertShader->GetID());
                    glAttachShader(ShaderProg.ProgramID, FragShader->GetID());
                    glLinkProgram(ShaderProg.ProgramID);
                    //checkCompileErrors(ShaderProg.ProgramID, "PROGRAM");
                    glValidateProgram(ShaderProg.ProgramID);
                    it.entity(i).remove<NewShaderProgram>();
                }
            });

    render_ecs.system<Texture, NewTexture>("Load Textures")
            .iter([](flecs::iter it, Texture* tex, NewTexture* new_tex){
                ZoneScopedN("Load Textures");
                for (auto i : it) {
                    auto &current_tex = tex[i];
                    current_tex.load(current_tex.filename);
                    it.entity(i).remove<NewTexture>();
                }
            });

    render_ecs.system("BeginRender")
        .iter([](flecs::iter it) {
            using namespace std::chrono_literals;
            ZoneScopedN("BeginRender");
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        });

    render_ecs.system<Camera>("RenderMeshes")
            .iter([&](flecs::iter it, Camera* cameras) {
                ZoneScopedN("RenderMeshes");
                auto ProgId = ProgramEntity.get<ShaderProgram>()->ProgramID;
                glUseProgram(ProgId);
                for (auto i : it) {
                    Camera& cam = cameras[i];

                    model_query.iter([&](flecs::iter it, Model* mesh) {
                        for (auto& current_mesh : mesh->meshes) {
                            glBindVertexArray(current_mesh.VAO);
                            //Temporary only one mesh
                            auto meshMatrix = glm::mat4(1.0);
                            auto ModelLocation = glGetUniformLocation(ProgId, "model");
                            glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, &meshMatrix[0][0]);
                            auto viewLocation = glGetUniformLocation(ProgId, "view");
                            glUniformMatrix4fv(glGetUniformLocation(ProgId, "view"), 1, GL_FALSE, &cam.view[0][0]);
                            auto projectionLocation = glGetUniformLocation(ProgId, "projection");
                            glUniformMatrix4fv(glGetUniformLocation(ProgId, "projection"), 1, GL_FALSE, &cam.projection[0][0]);
                            glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(current_mesh.indices.size()), GL_UNSIGNED_INT, 0);
                            //This does not work, for some reason
                            //glDrawArrays(GL_TRIANGLES, 0, current_mesh.indices.size());
                            //glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(current_mesh.indices.size()), GL_UNSIGNED_INT, 0);
                            glBindVertexArray(0);

                            // always good practice to set everything back to defaults once configured.
                            glActiveTexture(GL_TEXTURE0);
                        }
                    });
                }
            });

//    render_ecs.system("TestRender")
//            .iter([&](flecs::iter it) {
//                ZoneScopedN("TestRender");
//                // draw our first triangle
//                glUseProgram(shaderProgram);
//                glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
//                //glDrawArrays(GL_TRIANGLES, 0, 6);
//                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
//                // glBindVertexArray(0); // no need to unbind it every time
//            });

    render_ecs.system("EndRender")
            .iter([&](flecs::iter it) {
                ZoneScopedN("EndRender");
                SDL_GL_SwapWindow(Window);
            });
}

void GLRenderer::RenderThreadFunction()
{
    init();

    for( ; ; )
    {
        if (ShouldRender) {
            render_ecs.progress(0.033);
            ShouldRender = false;
            IsRenderingFinished = true;
            cv.notify_one();
        } else {
            std::unique_lock lk(RenderThreadLock);
            cv.wait(lk);
        }

        if (Running == 0){
            break;
        }
    }
}

void GLRenderer::TriggerRender() {
    std::unique_lock lk(RenderThreadLock);
    ShouldRender = true;
    cv.notify_one();
}

void GLRenderer::WaitForRenderEnd() {
    std::unique_lock lk(RenderThreadLock);
    if (!IsRenderingFinished) {
        cv.wait(lk);
    }

}


flecs::entity GLRenderer::GetTexture(const std::string &filename) {
    std::lock_guard g(RenderThreadLock);
    auto ent = texture_query.find([&filename](Texture &tex) {
        return tex.filename == filename;
    });

    if (ent == flecs::entity::null()) {
        auto NewTextureEntity = render_ecs.entity();
        NewTextureEntity
            .add<Texture>()
            .add<NewTexture>();

        auto Tex = NewTextureEntity.get_mut<Texture>();
        Tex->filename = filename;
        return NewTextureEntity;
    }
    return ent;
}

flecs::entity GLRenderer::GetShader(const std::string &filename) {
    std::lock_guard g(RenderThreadLock);
    auto ent = shader_query.find([&filename](Shader &tex) {
        return tex.filename == filename;
    });

    if (ent == flecs::entity::null()) {
        auto NewTextureEntity = render_ecs.entity();
        NewTextureEntity
                .add<Shader>()
                .add<NewShader>();

        auto Tex = NewTextureEntity.get_mut<Shader>();
        Tex->filename = filename;
        return NewTextureEntity;
    }
    return ent;
}

flecs::entity GLRenderer::GetModel(const std::string &filename) {
    std::lock_guard g(RenderThreadLock);
    auto ent = model_query.find([&filename](Model &tex) {
        return tex.filename == filename;
    });

    if (ent == flecs::entity::null()) {
        auto NewModelEntity = render_ecs.entity();
        NewModelEntity
                .add<Model>()
                .add<NewModel>();

        auto pModel = NewModelEntity.get_mut<Model>();
        pModel->filename = filename;
        return NewModelEntity;
    }
    return ent;
}

bool GLRenderer::initGL() {
    //Success flag
    bool SuccessResult = true;

    glEnable(GL_DEPTH_TEST);

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    return true;
}

void GLRenderer::printProgramLog( GLuint program )
{
    //Make sure name is shader
    if( glIsProgram( program ) )
    {
        //Program log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetProgramiv( program, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetProgramInfoLog( program, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a program\n", program );
    }
}

void GLRenderer::printShaderLog( GLuint shader )
{
    //Make sure name is shader
    if( glIsShader( shader ) )
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;

        //Get info string length
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );

        //Allocate string
        char* infoLog = new char[ maxLength ];

        //Get info log
        glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog );
        if( infoLogLength > 0 )
        {
            //Print Log
            printf( "%s\n", infoLog );
        }

        //Deallocate string
        delete[] infoLog;
    }
    else
    {
        printf( "Name %d is not a shader\n", shader );
    }
}

void GLRenderer::render()
{
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
//        if (Event.type == SDL_KEYDOWN)
//        {
//            switch (Event.key.keysym.sym)
//            {
//                case SDLK_ESCAPE:
//                    //Running = 0;
//                    break;
//                case 'f':
//                    FullScreen = !FullScreen;
//                    if (FullScreen)
//                    {
//                        SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
//                    }
//                    else
//                    {
//                        SDL_SetWindowFullscreen(Window, WindowFlags);
//                    }
//                    break;
//                default:
//                    break;
//            }
//        }
        if (Event.type == SDL_EVENT_QUIT)
        {
            Running = 0;
        }
    }

    ZoneScopedN("Render");
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    SDL_GL_SwapWindow(Window);
}

GLRenderer *GLRenderer::Get() {
    if (Singleton == nullptr) {
        Singleton = new GLRenderer();
    }
    return Singleton;
}

