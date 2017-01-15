#include "stdafx.h"
#include "PointMass.h"

#define AIRRESISTANCE 0.005f
#define SMOOTHING_FACTOR 0.7f

const static glm::vec3 gravity(0, -0.1, 0);

PointMass::PointMass()
{
	position = glm::vec3(1.5,1.5,1.5);
	lastPosition = glm::vec3(0,0,0);
	velocity = glm::vec3(0,0,0);
	acceleration = glm::vec3(0,0,0);
	anchored = false;
}

PointMass::PointMass(float x, float y, float z)
{
	position     = glm::vec3(x, y, z);
	lastPosition = glm::vec3(x, y, z);
	velocity     = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
	anchored     = false;
}


PointMass::~PointMass()
{
}

void PointMass::updateVelocity(unsigned int delta)
{
	float dt = delta * 0.001f;

	if (!anchored && dt != 0) {
		glm::vec3 oldVelocity = velocity;
		velocity = glm::mix((position - lastPosition)/dt * (1.0f - AIRRESISTANCE), oldVelocity, SMOOTHING_FACTOR);
		acceleration = (gravity + glm::vec3(velocity - oldVelocity)) / dt;
	}
}

void PointMass::updatePosition(unsigned int delta)
{
	float dt = delta * 0.001f;

	if (!anchored && dt != 0) {
		lastPosition = position;
		position = position + velocity * dt;
		velocity += acceleration * dt;
	}
}

void PointMass::reset(glm::vec3 position)
{
	this->position = position;
	lastPosition = position;
	velocity = glm::vec3(0, 0, 0);
	acceleration = glm::vec3(0, 0, 0);
}