//
// Created by wysoc on 10/28/2023.
//

#ifndef SPACESTATIONTOURISM_MODEL_H
#define SPACESTATIONTOURISM_MODEL_H

#include <vector>
#include <string>
#include "Mesh.h"
#include "assimp/material.h"

class aiNode;
class aiMesh;
struct aiScene;

struct NewModel{};

class Model
{
public:
//    Model(char *path)
//    {
//        loadModel(path);
//    }
    //void Draw(Shader &shader);
public:
    std::string filename;
public:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
                                         const std::string &typeName);
};


#endif //SPACESTATIONTOURISM_MODEL_H
