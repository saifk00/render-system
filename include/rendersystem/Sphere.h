#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "Shader.h"
#include "Mesh.h"
#include "Model.h"
#include "stb_image.h"
#include <cmath>
#include <vector>
#include <iostream>

class Sphere {
    std::unique_ptr<Mesh> mesh = nullptr;

    glm::vec3 position;
    glm::vec3 axis;
    glm::vec3 color;
    float angle; // clockwise angle about the axis
    float radius;
    int resolution;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    const GLenum TriangleMode = GL_TRIANGLE_STRIP;

    // sets up *indices[_count]* and *vertex[_count]*
    void generateVertexInfo() {
        std::vector<Vertex> tmp_vertices;
        std::vector<unsigned int> tmp_indices;

        glm::vec3 r_vec = position - glm::vec3(0, radius, 0); // bottom of the sphere

        int divsPhi = 10;
        int divsTheta = 20;

        if (resolution >= 0) {
            switch (resolution) {
            case 0:
                divsPhi = 20;
                divsTheta = 40;
                break;
            case 1:
                divsPhi = 36;
                divsTheta = 72;
                break;
            default:
                divsPhi = 90;
                divsTheta = 180;
                break;
            }
        }


        float rho = radius;
        for (int l = 0; l <= divsPhi; l++) { // layer (starting at 0 at the bottom and 180/d at the top)
            float phi = glm::radians(l * (180.0f / divsPhi));

            // round sin(phi)
            for (int t = 0; t <= divsTheta; t++) {
                float theta = glm::radians(t  * (360.0f / divsTheta));
                auto x = rho * sin(phi) * cos(theta);
                auto y = rho * sin(phi) * sin(theta);
                auto z = rho * cos(phi);
                auto pos = glm::vec3(x, y, z);
                auto d = - glm::normalize(pos);
                auto u = 0.5 + std::atan2(d.x, d.z) / (2 * 3.14159);
                auto v = 0.5 - std::asin(d.y) / 3.14159;

                auto tex = glm::vec2(u, v);
                Vertex vert;
                vert.Position = pos;
                vert.Normal = pos;
                vert.TexCoords = tex;
                tmp_vertices.push_back(vert);
                
                if (l == 0 || t == divsTheta) continue;

                // using triangle strip, so draw them in 'backwards N'
                unsigned int base = l * (divsTheta + 1);
                tmp_indices.push_back(base + t);
                tmp_indices.push_back(base + t - divsTheta);
                tmp_indices.push_back(base + t + 1);
                tmp_indices.push_back(base + t - divsTheta + 1);
            }
        }

        this->vertices = tmp_vertices;
        this->indices = tmp_indices;
    }

public:
    Sphere(glm::vec3 position, glm::vec3 axis, glm::vec3 color, float radius, float angle=0, int resolution=3, std::string base_texture="") {
        this->position = position;
        this->axis = axis;
        this->color = color;
        this->radius = radius;
        this->angle = angle;
        this->resolution = resolution;

        generateVertexInfo();

        if (!base_texture.empty()) this->AddTexture(base_texture, "texture_diffuse");

        mesh = std::unique_ptr<Mesh>(new Mesh(vertices, indices, textures));
        mesh->DrawMode = TriangleMode;
    }

    ~Sphere() {
        mesh = nullptr;
    }

    void AddTexture(std::string file, std::string type) {
        if (file.empty() || type.empty()) return;

        Texture tex;
        tex.id = Model::TextureFromFile(file.c_str());
        tex.path = file;
        tex.type = type;

        textures.push_back(tex);

        if (mesh != nullptr) {
            this->mesh->textures.push_back(tex);
        }
    }

    void Draw(Shader& shader) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, (float)glm::radians(angle), axis);

        shader.setMat4("model", model);
        shader.setVec3("material.ambient", 0.0f, 0.0f, 0.0f);
        shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setFloat("material.shininess", 32.0f);
        
        mesh->Draw(shader);
    }

    void Rotate(float degrees) {
        angle += degrees;
        if (angle > 360.0) angle -= 360.0;
    }

    void SetAxis(glm::vec3 newAxis) {
        axis = newAxis;
    }
};