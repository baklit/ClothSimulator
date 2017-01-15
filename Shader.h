#pragma once

#include <string>
#include <GL/glew.h>

class Shader
{
public:
	Shader();
	Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, bool isSources);
	~Shader();

	void compile();
	void bind();
	void unbind();
	void addAttribute(const std::string &attributeName);

	GLint getUniformLocation(const std::string &uniformName);

private:

	void compilePath(const std::string& filePath, GLuint id);
	void compileFromSource(const std::string& source, GLuint id);
	void link();

	int numAtrributes;

	std::string vertexShaderPath;
	std::string fragmentShaderPath;

	GLuint programID;
	GLuint vertexID;
	GLuint fragmentID;
};

