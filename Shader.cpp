#include "stdafx.h"
#include "Shader.h"

#include <iostream>
#include <fstream>
#include <vector>

Shader::Shader()
{
}

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
	programID = glCreateProgram();
	this->vertexShaderPath = vertexShaderPath;
	this->fragmentShaderPath = fragmentShaderPath;

	numAtrributes = 0;

	compile();
	link();
}

Shader::Shader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, bool isSources)
{
	programID = glCreateProgram();
	vertexID = glCreateShader(GL_VERTEX_SHADER);
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	if (!vertexID || !fragmentID) {
		std::cout << "Error creating shaders!" << std::endl;
		getchar();
	}

	numAtrributes = 0;

	compileFromSource(vertexShaderSource, vertexID);
	compileFromSource(fragmentShaderSource, fragmentID);
	link();
}


Shader::~Shader()
{
}

void Shader::compile()
{
	vertexID = glCreateShader(GL_VERTEX_SHADER);
	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);

	if (!vertexID || !fragmentID) {
		std::cout << "Error creating shaders!" << std::endl;
		getchar();
	}

	compilePath(vertexShaderPath, vertexID);
	compilePath(fragmentShaderPath, fragmentID);
}

void Shader::compileFromSource(const std::string& source, GLuint id)
{
	const char* contentsPtr = source.c_str();
	glShaderSource(id, 1, &contentsPtr, nullptr);
	glCompileShader(id);

	GLint isCompiled = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		std::cout << "Shader compile error for inbuilt shader" << std::endl;
		std::cout << "========================= SHADER SOURCE ==========================" << std::endl;
		std::cout << source << std::endl;
		std::cout << "==================================================================" << std::endl;
	}
	else {
		std::cout << "Shader compiled fine (inbuilt)" << std::endl;
	}
}

void Shader::compilePath(const std::string& filePath, GLuint id)
{
	std::ifstream shaderFile(filePath);
	std::string fileContents;
	std::string line;

	if (!shaderFile) { 
		std::cout << "Failed to open shaderfile (" << filePath << ")" << std::endl; 
	}
	else {

		while (std::getline(shaderFile, line)) {
			fileContents += line + "\n";
		}

		shaderFile.close();

		const char* contentsPtr = fileContents.c_str();
		glShaderSource(id, 1, &contentsPtr, nullptr);
		glCompileShader(id);

		GLint isCompiled = 0;
		glGetShaderiv(id, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE) {
			std::cout << "Shader compile error for " + filePath << std::endl;
			std::cout << "========================= SHADER SOURCE ==========================" << std::endl;
			std::cout << fileContents << std::endl;
			std::cout << "========================= SHADER ERROR  ==========================" << std::endl;
			GLint maxLength = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

			//The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(id, maxLength, &maxLength, &infoLog[0]);
			std::cout << &infoLog[0] << std::endl;
			std::cout << "==================================================================" << std::endl;
		}
		else {
			std::cout << "Shader compiled fine " << filePath << std::endl;
		}
	}
}

void Shader::link()
{
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);

	glLinkProgram(programID);

	GLint isLinked = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE) {
		std::cout << "Shader failed to link!" << std::endl;

		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);
		std::cout << &infoLog[0] << std::endl;
		getchar();
	}
	else {
		std::cout << "Shader linked fine!" << std::endl;
	}

	glDetachShader(programID, vertexID);
	glDetachShader(programID, fragmentID);
	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}

void Shader::addAttribute(const std::string &attributeName)
{
	glBindAttribLocation(programID, numAtrributes++, attributeName.c_str());
}

GLint Shader::getUniformLocation(const std::string &uniformName)
{
	return glGetUniformLocation(programID, uniformName.c_str());
}

void Shader::bind()
{
	glUseProgram(programID);
	for (int i = 0; i < numAtrributes; i++) {
		glEnableVertexAttribArray(i);
	}
}

void Shader::unbind()
{
	glUseProgram(0);
	for (int i = 0; i < numAtrributes; i++) {
		glDisableVertexAttribArray(i);
	}
}
