#include <rendersystem/Lights.h>


PointLight PointLight::DefaultPointLight()
{
    return PointLight(
        glm::vec3(0.0f, 1.0f, 0.0f),
        QuadAttenuation(1.0f, 0.07f, 0.017f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    );
}

void PointLight::SetPosition(glm::vec3 pos)
{
    Position = pos;
}

// TODO: this shouldn't take in an index. Create a lightingsystem class that manages lights.
void PointLight::SetShader(Shader& shader, int index)
{
    auto flPrefix = struct_name + "[" + std::to_string(index) + "]";

    shader.setBool(flPrefix + ".activated", true);
    shader.setVec3(flPrefix + ".position", Position);
    shader.setFloat(flPrefix + ".constant", Attenuation.constant);
    shader.setFloat(flPrefix + ".linear", Attenuation.linear);
    shader.setFloat(flPrefix + ".quadratic", Attenuation.quadratic);
    shader.setVec3(flPrefix + ".ambient", Ambient);
    shader.setVec3(flPrefix + ".diffuse", Diffuse);
    shader.setVec3(flPrefix + ".specular", Specular);
}

void DirectionalLight::SetDirection(glm::vec3 dir) {
    Direction = dir;
}

void DirectionalLight::SetShader(Shader& shader, int index) {
    auto flPrefix = struct_name + "[" + std::to_string(index) + "]";

    shader.setBool(flPrefix + ".activated", true);
    shader.setVec3(flPrefix + ".direction", Direction);
    shader.setVec3(flPrefix + ".ambient", Ambient);
    shader.setVec3(flPrefix + ".diffuse", Diffuse);
    shader.setVec3(flPrefix + ".specular", Specular);
}

DirectionalLight DirectionalLight::DefaultDirectionalLight() {
    return DirectionalLight(
        glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(0.1f),
        glm::vec3(0.5f),
        glm::vec3(0.5f)
    );
}