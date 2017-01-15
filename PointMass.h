#pragma once
#include <glm/glm.hpp>

class PointMass
{
public:
	PointMass();
	PointMass(float x, float y, float z);
	~PointMass();

	void updateVelocity(unsigned int delta);
	void updatePosition(unsigned int delta);
	void reset(glm::vec3 position);

	bool anchored;

	glm::vec3 position;

private:
	glm::vec3 lastPosition;
	glm::vec3 velocity;
	glm::vec3 acceleration;
};

