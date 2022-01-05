#pragma once
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <rendersystem/Shader.h>


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex) : Position(pos), Normal(norm), TexCoords(tex) {}
    Vertex() = default;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh(const Mesh& other);

    void Draw(Shader& shader);
protected:
    bool textures_dirty = true;

    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int DrawMode = GL_TRIANGLES;

    //  render data
    unsigned int VAO, VBO, EBO;

    // sets vao vbo ebo from vertices and indices
    void setupMesh();
};

// a mesh that can be positioned, rotated and scaled
class ControlledMesh : Mesh {
public:
    ControlledMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) : Mesh(vertices, indices) {}
    ControlledMesh(const ControlledMesh& other);
    
    void Draw(Shader& shader);

    static ControlledMesh CreateSphere(float radius, int resolution=3);
    static ControlledMesh CreateCuboid(float length, float height, float depth);
    static ControlledMesh CreateCube(float size);
    static ControlledMesh CreateQuad(float length, float height);


    void AddTexture(const std::string& texture_path, const std::string& texture_type);

    void SetColor(const glm::vec3& color);

    void Rotate(float degrees);
    void SetScale(float scale);
    void SetAxis(const glm::vec3& axis);
    void SetPosition(const glm::vec3& pos);

private:
    // transformation data
    glm::vec3 axis = glm::vec3(0, 1, 0);
    float angle = 0;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(0.0f);
    float scale = 1.0f;

};

