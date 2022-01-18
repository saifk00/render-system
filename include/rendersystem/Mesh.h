#pragma once
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <rendersystem/Shader.h>
#include <functional>


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

class Drawable {
public:
    virtual void Draw(const Shader& shader) = 0;
    virtual bool IsOpaque() = 0;
    virtual glm::vec3 Position() = 0;
};

class Mesh : public Drawable {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh(const Mesh& other);

    void Draw(const Shader& shader) override;
    bool IsOpaque() {
        return opaque_;
    }

    glm::vec3 Position() {
        return glm::vec3(0, 0, 0);
    }

protected:
    bool textures_dirty = true;

    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int DrawMode = GL_TRIANGLES;

    //  render data
    unsigned int VAO, VBO, EBO;
    bool opaque_ = true;

    // sets vao vbo ebo from vertices and indices
    void setupMesh();
};

// a mesh that can be positioned, rotated and scaled
class ControlledMesh : public Mesh {
public:
    ControlledMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool opaque=true) : Mesh(vertices, indices) {
        opaque_ = opaque;
    }
    ControlledMesh(const ControlledMesh& other);
    
    void Draw(const Shader& shader) override;

    static ControlledMesh CreateSphere(float radius, int resolution=3, bool opaque=true);
    static ControlledMesh CreateCuboid(float length, float height, float depth, bool opaque=true);
    static ControlledMesh CreateCube(float size, bool opaque=true);
    static ControlledMesh CreateQuad(float length, float height, bool opaque=true);


    void AddTexture(const std::string& texture_path,
        const std::string& texture_type,
        std::function<void(void)> textureSettingsCallback = []() {});

    void SetColor(const glm::vec3& color);

    void Rotate(float degrees);
    void SetScale(float scale);
    void SetAxis(const glm::vec3& axis);
    void SetPosition(const glm::vec3& pos);

    glm::vec3 Position() {
        return position;
    }

private:
    // transformation data
    glm::vec3 axis = glm::vec3(0, 1, 0);
    float angle = 0;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 color = glm::vec3(0.0f);
    float scale = 1.0f;

};

