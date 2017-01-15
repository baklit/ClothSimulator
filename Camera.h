#pragma once

#include <math.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

class Camera
{
public:
	Camera();
	~Camera();

	void computeMatricesFromInputs(unsigned int delta, float deltaMousex, float deltaMousey);
	void keyDown(SDL_Keysym key);
	void keyUp(SDL_Keysym key);

	glm::vec3 pos;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	glm::vec3 direction;
	glm::vec3 right;
	glm::vec3 up;

private:
	float horizontalAngle = 0.0f;
	float verticleAngle = 0.0f;
	float zVel = 0.0;
	float xVel = 0.0;
	int mod = 0;

};

