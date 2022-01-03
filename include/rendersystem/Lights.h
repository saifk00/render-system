#pragma once

#include "glm/glm.hpp"
#include "Shader.h"
#include <iostream>

struct QuadAttenuation {
	float constant;
	float linear;
	float quadratic;

	QuadAttenuation() : constant(0.0f), linear(0.0f), quadratic(0.0f) {

	}

	QuadAttenuation(float c, float l, float q) : 
		constant(c),
		linear(l),
		quadratic(q) {

	}
};

class PointLight {
	const std::string struct_name = "pointLights";

	glm::vec3 Position;
	QuadAttenuation Attenuation;
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;

public:
	static PointLight DefaultPointLight() {
		return PointLight(
			glm::vec3(0.0f, 1.0f, 0.0f),
			QuadAttenuation(1.0f, 0.07f, 0.017f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		);
	}

	PointLight(glm::vec3 position,
		QuadAttenuation attenuation,
		glm::vec3 ambient,
		glm::vec3 diffuse,
		glm::vec3 specular) {

		Position = position;
		Attenuation = attenuation;
		Ambient = ambient;
		Diffuse = diffuse;
		Specular = specular;
	}

	void SetPosition(glm::vec3 pos) {
		Position = pos;
	}

	void SetShader(Shader& shader, int index) {
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
};