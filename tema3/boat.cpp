#include "boat.h"
#include <stdlib.h>

Boat::Boat() {
	int dir = rand() % 2;
	if (dir == 0)
		direction = -1;
	else
		direction = 1;

	angle = RADIANS(rand() % 360);
	radius = 7.f + ((float)rand() / RAND_MAX) * (15.0f - 7.0f);
	speed = 10 + rand() % 5;
	position = glm::vec3(radius * glm::cos(angle), -0.5f, radius * glm::sin(angle));
}
