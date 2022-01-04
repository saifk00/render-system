#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <rendersystem/Shader.h>
#include <rendersystem/Mesh.h>
#include <rendersystem/Model.h>
#include <stb_image.h>
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

class Sphere {
    std::unique_ptr<Mesh> mesh = nullptr;

    glm::vec3 position;
    glm::vec3 axis;
    glm::vec3 color;
    float angle; // clockwise angle about the axis
    float radius;
    int resolution;
    float scale;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    const GLenum TriangleMode = GL_TRIANGLE_STRIP;

    // sets up *indices[_count]* and *vertex[_count]*
    void generateVertexInfo();

public:
    Sphere(glm::vec3 position,
        glm::vec3 axis,
        glm::vec3 color,
        float radius,
        float angle=0,
        int resolution=3,
        const std::string& base_texture="");
    
    Sphere(const Sphere& other);

    void Draw(Shader& shader);

    void Rotate(float degrees);

    void AddTexture(const std::string& file, const std::string& type);

    void SetAxis(glm::vec3 newAxis);

    void Scale(float scale);
};