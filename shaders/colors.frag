#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct PointLight {
    bool activated;

    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS_MAX 4
uniform PointLight pointLights[NR_POINT_LIGHTS_MAX];

uniform Material material;

uniform float near = 0.1;
uniform float far = 100.0;
uniform bool enableVisualiseDepthBuffer = false;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // calculate the ambient color. material.ambient specifies the absorption of RGB,
    // so multiplying it by lightColor gives the ambient color.
    vec3 ambient = material.ambient * light.ambient;

    // compute the diffuse color.
    //   1. calculate the orientation of the surface relative to the *light*
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, lightDir), 0.0);

    //   2. calculate the attenuation (depends on distance from the light)
    float d = length(fragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear*d + light.quadratic * d * d);

    //  3. compute the overall diffuse color (light color attenuated * orientation attenuation * base diffuse color)
    vec3 diffuse = attenuation * light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));

    // compute the specular color.
    //   1. reflect the light direction about the surface normal (i.e. how reflections work in real life)
    vec3 reflectDir = reflect(-lightDir, norm);

    //   2. calculate specular factor. this depends on how aligned the view is with the reflection
    float specFactor = max(dot(viewDir, reflectDir), 0.0);
    //   3. compute the specular intensity (some power of the factor)
    float spec = pow(specFactor, material.shininess);

    //   4. compute the overall specular color (the spec intensity * the base specular color) * lightColor
    vec3 specular = vec3(texture(material.texture_specular1, TexCoords)) * spec * light.specular;  

    // combine the ambient, diffuse and specular colors
    return ambient + diffuse + specular;
}

float LinearizeDepth(float depth) {
    float z = 2.0 * depth - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    // apply any textures we need in addition to the phong lighting
    // info required for point lights
    //   1. calculate the orientation of the surface relative to the *camera*
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 light1 = CalcPointLight(pointLights[0], Normal, FragPos, viewDir);

    if (!enableVisualiseDepthBuffer) {
        FragColor = vec4(light1, 1.0);
    } else {
        FragColor = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1.0);
    }
}