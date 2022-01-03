#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;


uniform sampler2D texture1;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main() {
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    float d = length(lightDir);
    float attenuation = 1.0 / (1.0 + 0.35*d + 0.44 * d * d);
    vec3 diffuse = attenuation * diff * lightColor;

    FragColor = texture(texture1, TexCoords);//vec4(diffuse + ambient , 1.0) * color;
}