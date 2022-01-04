#include <rendersystem/Cuboid.h>
#include <rendersystem/Model.h>

#include <iostream>
#include <vector>
#include <string>

Cuboid::Cuboid(glm::vec3 axis,
        glm::vec3 color,
        float depth,
        float height,
        float length,
        float angle,
        const std::string& base_texture) {

    this->position = position;
    this->axis = axis;
    this->color = color;
    this->depth = depth;
    this->height = height;
    this->length = length;
    this->angle = angle;

    generateVertexInfo();

    if (!base_texture.empty()) {
        this->AddTexture(base_texture, "texture_diffuse");
    }

    mesh = std::make_unique<Mesh>(vertices, indices, textures);
    mesh->DrawMode = TriangleMode;
}


void Cuboid::generateVertexInfo() {
    vertices.clear();
    indices.clear();
    textures.clear();

    auto x = glm::vec3(length/2, 0, 0);
    auto y = glm::vec3(0, height/2, 0);
    auto z = glm::vec3(0, 0,  depth/2);

    // front face: position - z
    auto front_and_center = -z;
    auto back_and_center = +z;
    std::vector<glm::vec3> tmp_vertices{
        front_and_center - y - x, // bottom left
        front_and_center - y + x, // bottom right
        front_and_center + y + x, // top right
        front_and_center + y - x, // top left
        back_and_center - y - x, // bottom left
        back_and_center - y + x, // bottom right
        back_and_center + y + x, // top right
        back_and_center + y - x // top left
    };

    // front left top right bottom back
    std::vector<glm::vec3> normals {
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };

    // front left top right bottom back
    int base_index;
    
    // bl br tr tl
    auto push_back_vertices = [&](int a, int b, int c, int d, glm::vec3 normal) {
        vertices.push_back(Vertex(tmp_vertices[a], normal, glm::vec2(0.0f, 0.0f)));
        vertices.push_back(Vertex(tmp_vertices[b], normal, glm::vec2(1.0f, 0.0f)));
        vertices.push_back(Vertex(tmp_vertices[c], normal, glm::vec2(1.0f, 1.0f)));
        vertices.push_back(Vertex(tmp_vertices[d], normal, glm::vec2(0.0f, 1.0f)));
    };

    // front left top right bottom back
    push_back_vertices(0, 1, 2, 3, normals[0]);
    push_back_vertices(4, 0, 3, 7, normals[1]);
    push_back_vertices(7, 3, 2, 6, normals[2]);
    push_back_vertices(6, 2, 1, 5, normals[3]);
    push_back_vertices(5, 1, 0, 4, normals[4]);
    push_back_vertices(5, 4, 7, 6, normals[5]);

    for (unsigned int i = 0; i < 6; i++) {
        indices.insert(indices.end(), {
            i * 4 + 0,
            i * 4 + 1,
            i * 4 + 2,
            i * 4 + 0,
            i * 4 + 2,
            i * 4 + 3
        });
    }
}

void Cuboid::AddTexture(const std::string& texture_path, const std::string& texture_type) {
    if (texture_path.empty() || texture_type.empty()) {
        return;
    }

    Texture tex;
    tex.id = Model::TextureFromFile(texture_path.c_str());
    tex.path = texture_path;
    tex.type = texture_type;
    textures.push_back(tex);

    if (mesh != nullptr) {
        this->mesh->textures.push_back(tex);
    }
}

void Cuboid::Draw(Shader& shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, (float)glm::radians(angle), axis);

    shader.setMat4("model", model);
    shader.setVec3("material.ambient", color);
    shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    shader.setFloat("material.shininess", 32.0f);
    
    mesh->Draw(shader);
}

void Cuboid::SetPosition(glm::vec3 newPos) {
    position = newPos;
}