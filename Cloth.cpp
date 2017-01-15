#include "stdafx.h"
#include "Cloth.h"

#define OVERCONSTRAIN_FACTOR 1.0f

Cloth::Cloth(int width, int height, float scale)
{
	this->width = width;
	this->height = height;
	this->scale = scale;

	//points[width][height];
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			points.push_back(PointMass((float)x * scale * OVERCONSTRAIN_FACTOR, height, (float)y * scale));
		}
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (x < width-1) {
				constraints.push_back(LinkConstraint(&points[x + y*width], &points[x + 1 + y*width]));
			}
			if (y < height-1) {
				constraints.push_back(LinkConstraint(&points[x + y*width], &points[x + (y + 1)*width]));
			}
		}
	}

	points[height - 1].anchored = true;
	points[width - 1 + (height - 1)*width].anchored = true;

	//for (int x = 0; x < width; x++) {
		//points[x + height].anchored = true;
	//}
}


Cloth::~Cloth()
{
}

void Cloth::simulate(unsigned int delta)
{
	for (int iteration = 0; iteration < 3; iteration++) {
		for (int i = constraints.size(); i > 0; i--) {
			constraints[i - 1].solve(delta);
		}

		for (int i = 0; i < constraints.size(); i++) {
			constraints[i].solve(delta);
		}

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				points[x + y*width].updateVelocity(delta);
			}
		}

		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				points[x + y*width].updatePosition(delta);
			}
		}
	}
}

std::vector<PointMass>* Cloth::getPoints()
{
	return &points;
}

void Cloth::init()
{
	glGenBuffers(1, &vboID);
	glGenBuffers(1, &normalBufferID);
	glGenBuffers(1, &indexBufferID);
	glGenBuffers(1, &uvBufferID);
	
	for (int x = 0; x < width-1; x++) {
		for (int y = 0; y < height-1; y++) {
			indices.push_back(x + y*width);
			indices.push_back((x + 1) + y*width);
			indices.push_back(x + (y + 1)*width);
			indices.push_back(x + (y+1)*width);
			indices.push_back((x + 1) + y*width);
			indices.push_back((x + 1) + (y+1)*width);
		}
	}

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			uvCoords.push_back(glm::vec2(1.0f - ((float)x / width), 1.0f - ((float) y / height)));
		}
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glBufferData(GL_ARRAY_BUFFER, uvCoords.size() * sizeof(glm::vec2), &uvCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	vertices.resize(width * height, glm::vec3());
	normals.resize(width * height, glm::vec3());
}

void Cloth::reset()
{
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			if (!points[x*height + y].anchored) {
				points[x*height + y].reset(glm::vec3((float)x * scale * OVERCONSTRAIN_FACTOR, height, (float)y * scale));
			}
		}
	}
}

void Cloth::draw()
{
	//Prepare vertex and normal arrays for sending to the GPU
	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			vertices[x + y*width] = points[x + y*width].position;
		}
	}

	for (int x = 0; x < width-1; x++) {
		for (int y = 0; y < height-1; y++) {
			normals[x + y*width] = -glm::cross(points[x + (y + 1)*width].position - points[x + y*width].position, points[x+1 + y*width].position - points[x + y*width].position);
		}
	}

	for (int y = 0; y < height-1; y++) {
		normals[width-1 + y*width] = -glm::cross(points[width-2 + y*width].position - points[width-1 + y*width].position, points[width-1 + (y+1)*width].position - points[width-1 + y*width].position);
	}

	int y = height - 1;
	for (int x = 0; x < width-1; x++) {
		normals[x + y*width] = -glm::cross(points[x+1+y*width].position - points[x+y*width].position, points[x + (y-1)*width].position - points[x + y*width].position);
	}

	//Send the vertex locations to the GPU
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * width * height, &vertices[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//Send the vertex normals to the GPU
	glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * width * height, &normals[0], GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

	//Send draw call to the GPU
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);

	//Unbind data to free the GPU for other uses
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
