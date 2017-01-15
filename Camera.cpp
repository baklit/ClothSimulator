#include "stdafx.h"
#include "Camera.h"
#include <glm/gtx/transform.hpp>
#include <iostream>


Camera::Camera()
{
	this->pos = glm::vec3(2, 2, 2);
}

Camera::~Camera()
{
}

void Camera::computeMatricesFromInputs(unsigned int delta, float deltaMousex, float deltaMousey)
{
	horizontalAngle -= 0.002f * deltaMousex;
	verticleAngle -= 0.002f * deltaMousey;

	if (verticleAngle > 1.4f) {
		verticleAngle = 1.4f;
	}
	else if (verticleAngle < -1.4f) {
		verticleAngle = -1.4f;
	}

	direction = glm::vec3(
		cos(verticleAngle) * sin(horizontalAngle),
		sin(verticleAngle),
		cos(verticleAngle) * cos(horizontalAngle));

	right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f));

	up = glm::cross(right, direction);

	pos += direction * (zVel * delta * 0.005f * mod);
	pos += right * (xVel * delta * 0.005f * mod);

	projectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.01f, 1000.0f);
	viewMatrix = glm::lookAt(pos, pos + direction, up);
}

void Camera::keyDown(SDL_Keysym key)
{
	switch (key.sym) {
	case SDLK_w:
		zVel = 1;
		break;
	case SDLK_s:
		zVel = -1;
		break;
	case SDLK_d:
		xVel = 1;
		break;
	case SDLK_a:
		xVel = -1;
		break;
	case SDLK_LSHIFT:
		mod = 5;
		break;
	}
}

void Camera::keyUp(SDL_Keysym key)
{
	switch (key.sym) {
	case SDLK_w:
		zVel = 0;
		break;
	case SDLK_s:
		zVel = 0;
		break;
	case SDLK_d:
		xVel = 0;
		break;
	case SDLK_a:
		xVel = 0;
		break;
	case SDLK_LSHIFT:
		mod = 1;
		break;
	}
}
