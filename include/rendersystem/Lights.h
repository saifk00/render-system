#pragma once

#include <glm/glm.hpp>
#include <rendersystem/Shader.h>
#include <iostream>

struct QuadAttenuation {
	float constant;
	float linear;
	float quadratic;

	QuadAttenuation() : constant(0.0f), linear(0.0f), quadratic(0.0f) {}

	QuadAttenuation(float c, float l, float q) : 
		constant(c),
		linear(l),
		quadratic(q) {}
};

class PointLight {
	const std::string struct_name = "pointLights";

	glm::vec3 Position;
	QuadAttenuation Attenuation;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

public:
	static PointLight DefaultPointLight();

	PointLight(glm::vec3 position,
		QuadAttenuation attenuation,
		glm::vec3 ambient,
		glm::vec3 diffuse,
		glm::vec3 specular) : Position(position),
			Attenuation(attenuation),
			Ambient(ambient),
			Diffuse(diffuse),
			Specular(specular) {}

	void SetPosition(glm::vec3 pos);

	void SetShader(Shader& shader, int index);
};

class DirectionalLight {
	const std::string struct_name = "directionalLights";

	glm::vec3 Direction;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
public:
	static DirectionalLight DefaultDirectionalLight();

	DirectionalLight(glm::vec3 direction,
		glm::vec3 ambient,
		glm::vec3 diffuse,
		glm::vec3 specular) : Direction(direction),
			Ambient(ambient),
			Diffuse(diffuse),
			Specular(specular) {}

	void SetDirection(glm::vec3 dir);

	void SetShader(Shader& shader, int index);
};