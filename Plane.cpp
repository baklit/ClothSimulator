#include "stdafx.h"
#include "Plane.h"

#include <glm/glm.hpp>


Plane::Plane()
{
}


Plane::~Plane()
{
}

void Plane::init()
{
	glm::vec3 vertices[] = {
		glm::vec3(-100.0f,-2.0f, -100.0f),
		glm::vec3(-100.0f,-2.0f,  100.0f),
		glm::vec3(100.0f, -2.0f,  100.0f),
		glm::vec3(100.0f, -2.0f, -100.0f),
	};

	glm::vec3 normals[] = {
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
	};

	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, &vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &normalID);
	glBindBuffer(GL_ARRAY_BUFFER, normalID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, &normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Plane::draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, normalID);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glDrawArrays(GL_QUADS, 0, 4);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
