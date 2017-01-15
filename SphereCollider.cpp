#include "stdafx.h"
#include "SphereCollider.h"
#include <vector>
#include <math.h>

#define HEIGHT_DIVISIONS 10
#define WIDTH_DIVISIONS 10

SphereCollider::SphereCollider(float radius, glm::vec3 startPosition, glm::vec3 direction, Cloth* cloth)
{
	this->radius = radius;
	this->position = startPosition;
	this->direction = direction;
	this->interactionPoints = cloth->getPoints();
}


SphereCollider::~SphereCollider()
{
}

void SphereCollider::init()
{
	vertices.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	uvCoords.push_back(glm::vec2(0.0f, 0.0f));
	normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	for (int y = 1; y < HEIGHT_DIVISIONS; y++) {
		for (int x = 0; x < WIDTH_DIVISIONS; x++) {
			vertices.push_back(glm::vec3(
				std::cos((x / (WIDTH_DIVISIONS * 0.5f)) * 3.14f) * std::sin((y / (HEIGHT_DIVISIONS * 0.5f)) * 1.57f),
				std::cos((y / (HEIGHT_DIVISIONS * 0.5f)) * 1.57f),
				std::sin((x / (WIDTH_DIVISIONS * 0.5f)) * 3.14f) * std::sin((y / (HEIGHT_DIVISIONS * 0.5f)) * 1.57f)
				));
			uvCoords.push_back(glm::vec2((float) x / WIDTH_DIVISIONS, (float) y / HEIGHT_DIVISIONS));
			normals.push_back(vertices.back());
		}
	}

	vertices.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	uvCoords.push_back(glm::vec2(1.0f, 1.0f));
	normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));

	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normalID);
	glBindBuffer(GL_ARRAY_BUFFER, normalID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &uvID);
	glBindBuffer(GL_ARRAY_BUFFER, uvID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvCoords.size(), &uvCoords[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	indices.push_back(0);

	for (int x = 0; x < WIDTH_DIVISIONS-1; x++) {
		indices.push_back(x+1);
		indices.push_back(x+2);
		indices.push_back(0);
	}

	indices.push_back(1);
	indices.push_back(WIDTH_DIVISIONS);

	for (int y = 1; y < HEIGHT_DIVISIONS-1; y++) {
		for (int x = -1; x < WIDTH_DIVISIONS; x++) {
			indices.push_back(y*WIDTH_DIVISIONS + x + 1);
			indices.push_back(((y-1) * WIDTH_DIVISIONS) + x + 2);
		}
	}

	indices.push_back(WIDTH_DIVISIONS * (HEIGHT_DIVISIONS-1) + 1);

	for (int x = 0; x < WIDTH_DIVISIONS - 1; x++) {
		indices.push_back(WIDTH_DIVISIONS * (HEIGHT_DIVISIONS - 2) + x + 1);
		indices.push_back(WIDTH_DIVISIONS * (HEIGHT_DIVISIONS - 2) + x + 2);
		indices.push_back(WIDTH_DIVISIONS * (HEIGHT_DIVISIONS - 1) + 1);
	}

	glGenBuffers(1, &elementID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SphereCollider::simulate(unsigned int delta)
{
	position = position + direction * 0.001f * (float)delta;

	for (int i = 0; i < interactionPoints->size(); i++) {
		glm::vec3 difference = (*interactionPoints)[i].position - position;

		if (glm::length(difference) < radius) {
			(*interactionPoints)[i].position = position + glm::normalize(difference) * radius;
		}
	}
}


void SphereCollider::draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, normalID);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//glBindBuffer(GL_ARRAY_BUFFER, uvID);
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementID);

	glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}