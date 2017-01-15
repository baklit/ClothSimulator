#pragma once

#include <GL/glew.h>
#include <vector>
#include "PointMass.h"
#include "LinkConstraint.h"

class Cloth
{
public:
	Cloth(int width, int height, float scale);
	~Cloth();

	void draw();
	void init();
	void reset();
	void simulate(unsigned int delta);
	std::vector<PointMass>* getPoints();
	
private:
	int width;
	int height;

	float scale;

	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvCoords;
	std::vector<LinkConstraint> constraints;
	std::vector<PointMass> points;

	glm::vec3* bufferData;
	GLuint vboID;
	GLuint indexBufferID;
	GLuint normalBufferID;
	GLuint uvBufferID;
};

