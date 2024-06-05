#pragma once
#include "components/simple_scene.h"

class Boat {
public:
	Boat();

	float angle;
	float radius;
	int direction;
	float speed;
	glm::vec3 position;
	glm::vec3 previous_position;
};

