#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    vec3 ambient;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

uniform Material material;

void main() {
    vec4 texColor = texture(material.texture_diffuse1, TexCoords);
    if (texColor.a < 0.1)
        discard;
    FragColor = texColor;
}