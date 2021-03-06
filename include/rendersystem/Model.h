#ifndef _MODEL_H
#define _MODEL_H

#include <vector>
#include <iostream>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <rendersystem/Shader.h>
#include <rendersystem/Mesh.h>

class Model : public Drawable
{
public:
    Model(std::string path)
    {
        loadModel(path);
    }
    void Draw(const Shader& shader) override;

    void Rotate(float angle);

    bool IsOpaque() {
        return opaque_;
    }

    glm::vec3 Position() {
        return position;
    }

private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 axis = glm::vec3(0.0f, 1.0f, 0.0f);
    float angle = 0.0f;
    float scale = 1.0f;

    bool opaque_ = true;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(
        aiMaterial* mat,
        aiTextureType type,
        std::string typeName);  
};
#endif