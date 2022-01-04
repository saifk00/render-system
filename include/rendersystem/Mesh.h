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

    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int DrawMode = GL_TRIANGLES;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader& shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

