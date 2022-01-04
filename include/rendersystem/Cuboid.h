#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <memory>
#include <glad/glad.h>

#include <rendersystem/Shader.h>
#include <rendersystem/Mesh.h>

class Cuboid {
    std::unique_ptr<Mesh> mesh = nullptr;

    glm::vec3 position;
    glm::vec3 axis;
    glm::vec3 color;
    float angle;
    // x direction
    float length;
    // y direction
    float height;
    // z direction
    float depth;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    const GLenum TriangleMode = GL_TRIANGLES;

    void generateVertexInfo();

public:
    Cuboid(glm::vec3 axis,
        glm::vec3 color,
        float length,
        float height,
        float depth,
        float angle = 0,
        const std::string& base_texture = "");

    void Draw(Shader& shader);

    void AddTexture(const std::string& texture_path, const std::string& texture_type);
    
    void SetPosition(glm::vec3 newPos);
};