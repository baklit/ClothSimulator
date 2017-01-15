#pragma once

#include <GL/glew.h>

class Plane
{
public:
	Plane();
	~Plane();

	void draw();
	void init();

private:
	GLuint vboID;
	GLuint normalID;
};

