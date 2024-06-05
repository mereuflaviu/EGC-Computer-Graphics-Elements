#pragma once

#include "components/simple_scene.h"

struct Light {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 direction;
	float intensity;
	int type;
};
