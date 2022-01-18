#include <rendersystem/Mesh.h>
#include <rendersystem/Utils.h>
#include <rendersystem/Shader.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

Mesh::Mesh(std::vector<Vertex> vertices,
    std::vector<unsigned int> indices,
    std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
    this->vertices = vertices;
    this->indices = indices;

    setupMesh();
}

Mesh::Mesh(const Mesh& other) {
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->textures = other.textures;

    this->DrawMode = other.DrawMode;
    setupMesh();
}

ControlledMesh::ControlledMesh(const ControlledMesh& other) : Mesh(other) {
    this->axis = other.axis;
    this->angle = other.angle;
    this->position = other.position;
    this->color = other.color;
    this->scale = other.scale;
    this->opaque_ = other.opaque_;
}

void ControlledMesh::Rotate(float degrees) {
    angle += degrees;
    if (angle > 360.0f) {
        angle -= 360.0f;
    }
}

void ControlledMesh::SetColor(const glm::vec3& color) {
    this->color = color;
}

void ControlledMesh::SetScale(float scale) {
    this->scale = scale;
}

void ControlledMesh::SetAxis(const glm::vec3& axis) {
    this->axis = axis;
}

void ControlledMesh::SetPosition(const glm::vec3& pos) {
    this->position = pos;
}

void Mesh::Draw(const Shader& shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    // naming convention:
    //  texture_specularN  => specular map
    //  texture_diffuseN   => diffuse map
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);

        // if textures haven't changed, this doesnt need to be done
        if (Mesh::textures_dirty) {
            std::string name = textures[i].type;
            std::string num;
            if (name == "texture_diffuse") num = std::to_string(diffuseNr++);
            else if (name == "texture_specular") num = std::to_string(specularNr++);
            else std::cout << "ERROR: invalid texture name " << name << std::endl;

            auto prop = "material." + name + num;
            shader.setInt(prop.c_str(), i);

            textures_dirty = false;
        }
    }

    // now draw the mesh and unbind the VAO
    glBindVertexArray(VAO);
    glDrawElements(DrawMode, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ControlledMesh::Draw(const Shader& shader)
{
    // apply transformations
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, glm::vec3(scale));
    model = glm::rotate(model, (float)glm::radians(angle), axis);

    shader.setMat4("model", model);
    shader.setVec3("material.ambient", color);

    Mesh::Draw(shader);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // bind the VAO then {VBO, EBO} 
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // load the vertices into the VBO
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // load the indices into the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // unbind {EBO, VBO} then VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ControlledMesh::AddTexture(const std::string& texture_path,
    const std::string& texture_type,
    std::function<void(void)> textureSettingsCallback)
{
    // check if texture is already loaded
    for (unsigned int i = 0; i < textures.size(); i++) {
        if (textures[i].path == texture_path) {
            return;
        }
    }

    // if not, load it
    Texture texture;
    texture.id = Utils::TextureFromFile(texture_path, textureSettingsCallback);
    texture.type = texture_type;
    texture.path = texture_path;
    textures.push_back(texture);

    textures_dirty = true;
}


ControlledMesh ControlledMesh::CreateSphere(float radius, int resolution, bool opaque) {
    // step 1: generate vertices (incl. vertex|normal|texcoord) and indices
    std::vector<Vertex> tmp_vertices;
    std::vector<unsigned int> tmp_indices;

    glm::vec3 r_vec = -glm::vec3(0, radius, 0); // bottom of the sphere

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
            Vertex vert(pos, pos, tex);
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

    auto cmesh = ControlledMesh(tmp_vertices, tmp_indices, opaque);
    cmesh.DrawMode = GL_TRIANGLE_STRIP;

    return cmesh;
}

ControlledMesh ControlledMesh::CreateCuboid(float length, float height, float depth, bool opaque) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

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

    auto cmesh = ControlledMesh(vertices, indices, opaque);
    cmesh.DrawMode = GL_TRIANGLES;

    return cmesh;
}

ControlledMesh ControlledMesh::CreateCube(float size, bool opaque) {
    return ControlledMesh::CreateCuboid(size, size, size);
}

ControlledMesh ControlledMesh::CreateQuad(float length, float height, bool opaque) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    auto x = glm::vec3(length/2, 0, 0);
    auto y = glm::vec3(0, height/2, 0);

    // front side
    auto front_norm = glm::vec3(0.0f, 0.0f, -1.0f);
    vertices.push_back(Vertex(-x - y, front_norm, glm::vec2(0.0f, 0.0f)));
    vertices.push_back(Vertex(x - y, front_norm, glm::vec2(1.0f, 0.0f)));
    vertices.push_back(Vertex(x + y, front_norm, glm::vec2(1.0f, 1.0f)));
    vertices.push_back(Vertex(-x + y, front_norm, glm::vec2(0.0f, 1.0f)));

    // back side
    auto back_norm = glm::vec3(0.0f, 0.0f, 1.0f);
    vertices.push_back(Vertex(-x - y, back_norm, glm::vec2(0.0f, 0.0f)));
    vertices.push_back(Vertex(x - y, back_norm, glm::vec2(1.0f, 0.0f)));
    vertices.push_back(Vertex(x + y, back_norm, glm::vec2(1.0f, 1.0f)));
    vertices.push_back(Vertex(-x + y, back_norm, glm::vec2(0.0f, 1.0f)));

    // indices
    indices.insert(indices.end(), {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7
    });

    auto cmesh = ControlledMesh(vertices, indices, opaque);
    cmesh.DrawMode = GL_TRIANGLES;

    return cmesh;
}