#pragma once

#include <glm/glm.hpp>
#include "Cloth.h"

class SphereCollider
{
public:
	SphereCollider(float radius, glm::vec3 startPosition, glm::vec3 direction, Cloth* cloth);
	~SphereCollider();

	void simulate(unsigned int delta);
	void draw();
	void init();

	float radius;

	glm::vec3 position;
	glm::vec3 direction;

private:
	GLuint vboID;
	GLuint uvID;
	GLuint normalID;
	GLuint elementID;

	std::vector<PointMass>* interactionPoints;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvCoords;
	std::vector<unsigned int> indices;
};

