// ClothSimulator.cpp : Defines the entry point for the console application.
//


#include "Shader.h"
#include "Camera.h"
#include "Cloth.h"
#include "Plane.h"
#include "SphereCollider.h"

#include <iostream>
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <SDL2/SDL.h>

#define WIDTH 1024
#define HEIGHT 768

enum State{
	INIT,
	RUNNING,
	QUIT
};

void draw();
void processInputs(unsigned int delta);

Cloth cloth = Cloth(20, 20, 1.0f);
Plane plane;
SphereCollider sphere = SphereCollider(2.0f, glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 10.0f), &cloth);
SphereCollider sun = SphereCollider(2.0f, glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 10.0f), &cloth);
Camera camera = Camera();
State currentState = State::INIT;
Shader basicShader;
Shader shadowmapShader;
Shader godrayShader;

glm::vec3 sunPos = glm::vec3(0.0f, 20.0f, 20.0f) * 3.0f;
glm::vec3 sunDirection = sunPos / glm::length(sunPos);

GLuint vertexbuffer;
GLuint VPID;
GLuint ModelPosID;
GLuint ModelScaleID;
GLuint ModelColorID;
GLuint shadowModelPosID;
GLuint shadowModelScaleID;
GLuint sphereID;
GLuint textureID;
GLuint depthMVPID;
GLuint depthMVPID2;
GLuint sunDirID;
GLuint shadowFrameBuffer;
GLuint colorFrameBuffer;
GLuint depthTexture;
GLuint depthTexture2;

SDL_Window* window;

int main(int argc, char** argv)
{
	//Initialise SDL systems
	SDL_Init(SDL_INIT_EVERYTHING);

	//Create a window in Windows and bind an OpenGL context to it
	window = SDL_CreateWindow("Cloth Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	
	//Initialise OpenGL extension systems and finalise OpenGL Initialisation
	glewInit();
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	glClearColor(0.0f, 0.4f, 0.8f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//Compile and link shaders for use with OpenGL context
	basicShader = Shader("Shaders/BasicVertexShader.vert", "Shaders/BasicFragmentShader.frag");

	//Then add the attributes and uniforms we will be using
	basicShader.addAttribute("vertexPosition");
	basicShader.addAttribute("vertexNormal");
	VPID = basicShader.getUniformLocation("VP");
	ModelPosID = basicShader.getUniformLocation("modelPos");
	ModelScaleID = basicShader.getUniformLocation("modelScale");
	ModelColorID = basicShader.getUniformLocation("modelColor");
	depthMVPID2 = basicShader.getUniformLocation("depthMVP");
	sunDirID = basicShader.getUniformLocation("sunDirection");
	
	shadowmapShader = Shader("Shaders/ShadowmapVertexShader.vert", "Shaders/ShadowmapFragmentShader.frag");
	shadowmapShader.addAttribute("vertexPosition");
	depthMVPID = shadowmapShader.getUniformLocation("depthMVP");
	shadowModelPosID = shadowmapShader.getUniformLocation("modelPosition");
	shadowModelScaleID = shadowmapShader.getUniformLocation("modelScale");

	godrayShader = Shader("Shaders/GodRayVertexShader.vert", "Shaders/GodRayFragmentShader.frag");
	godrayShader.addAttribute("vertexPosition");

	//Send texture to GPU so fragment shader is able to use it
	SDL_Surface texture = *(SDL_LoadBMP_RW(SDL_RWFromFile("Resources/flag.bmp", "rb"), 1));
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.w, texture.h, 0, GL_BGR, GL_UNSIGNED_BYTE, texture.pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(basicShader.getUniformLocation("Texture"), 0);

	//////////////////////////////////////////////////////////////////////////////////////////////////
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

						   // Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Depth frame buffer failed to be created" << std::endl;
		getchar();
	}

	glGenFramebuffers(1, &colorFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, colorFrameBuffer);

	glActiveTexture(GL_TEXTURE2);
	GLuint colorTexture;
	glGenTextures(1, &colorTexture);
	glBindTexture(GL_TEXTURE_2D, colorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
	
	glDrawBuffer(GL_BACK);
	glActiveTexture(GL_TEXTURE3);
	glGenTextures(1, &depthTexture2);
	glBindTexture(GL_TEXTURE_2D, depthTexture2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	
	//glDrawBuffer(GL_NONE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture2, 0);

	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Color frame buffer failed to be created" << std::endl;
		getchar();
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////

	//Initialise our objects to prepare them for simulation
	cloth.init();
	sphere.init();
	plane.init();
	sun.init();

	//Get the start time of the program (to calculate frame delta's) and set it's state to running
	unsigned int time = SDL_GetTicks();
	unsigned int delta = 0;
	currentState = State::RUNNING;

	//Start the main program loop
	while (currentState != State::QUIT) {
		delta = SDL_GetTicks() - time;
		while (delta < 7) {
			SDL_Delay(8 - delta);
			delta = SDL_GetTicks() - time;
		}
		time = SDL_GetTicks();	

		std::cout << "Fps: " << 1.0f / (delta / 1000.0f) << "      \r" << std::flush;

		//delta = 8;

		sunPos = glm::vec3(200 * std::sin(SDL_GetTicks() * 0.001f), 200.0f, 200 * std::cos(SDL_GetTicks() * 0.001f));
		sunDirection = sunPos / glm::length(sunPos);

		sun.position = sunPos;

		processInputs(delta);
		cloth.simulate(delta);
		sphere.simulate(delta);
		draw();
	}

    return 0;
}


void draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT);

	shadowmapShader.bind();

	// Compute the MVP matrix from the light's point of view
	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-100, 100, -100, 100, 10, 400);
	//glm::mat4 depthProjectionMatrix = camera.projectionMatrix;
	glm::mat4 depthViewMatrix = glm::lookAt(sunPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//glm::mat4 depthViewMatrix = camera.viewMatrix;
	glm::mat4 depthModelMatrix = glm::mat4(1.0);
	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(depthMVPID, 1, GL_FALSE, &depthMVP[0][0]);
	glUniform3f(shadowModelPosID, 0.0f, 0.0f, 0.0f);
	glUniform3f(shadowModelScaleID, 1.0f, 1.0f, 1.0f);
	plane.draw();
	cloth.draw();
	glUniform3f(shadowModelPosID, sphere.position.x, sphere.position.y, sphere.position.z);
	glUniform3f(shadowModelScaleID, sphere.radius * 0.90f, sphere.radius * 0.90f, sphere.radius * 0.90f);
	sphere.draw();
	sun.draw();

	shadowmapShader.unbind();
	basicShader.bind();

	glUniform1i(basicShader.getUniformLocation("shadowTexture"), 1);

	glBindFramebuffer(GL_FRAMEBUFFER, colorFrameBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 bias(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);

	glm::mat4 projection = camera.projectionMatrix;
	glm::mat4 view = camera.viewMatrix;
	glm::mat4 vp = projection * view;
	glm::mat4 depthBiasMVP = bias * depthMVP;

	glUniformMatrix4fv(VPID, 1, GL_FALSE, &vp[0][0]);
	glUniformMatrix4fv(depthMVPID2, 1, GL_FALSE, &depthBiasMVP[0][0]);
	glUniform3f(sunDirID, sunDirection.x, sunDirection.y, sunDirection.z);

	glUniform3f(ModelPosID, 0.0f, 0.0f, 0.0f);
	glUniform3f(ModelScaleID, 1.0f, 1.0f, 1.0f);
	glUniform3f(ModelColorID, 1.0f, 1.0f, 1.0f);
	cloth.draw();
	glUniform3f(ModelColorID, 0.0f, 0.7f, 0.05f);
	plane.draw();
	glUniform3f(ModelPosID, sphere.position.x, sphere.position.y, sphere.position.z);
	glUniform3f(ModelScaleID, sphere.radius * 0.90f, sphere.radius * 0.90f, sphere.radius * 0.90f);
	glUniform3f(ModelColorID, 1.0f, 1.0f, 1.0f);
	sphere.draw();
	
	glUniform3f(ModelPosID, sun.position.x, sun.position.y, sun.position.z);
	glUniform3f(ModelScaleID, sun.radius * 0.90f, sun.radius * 0.90f, sun.radius * 0.90f);
	sun.draw();

	basicShader.unbind();

	godrayShader.bind();

	glm::vec4 SunScreenPos = vp * glm::vec4(sunPos, 0.0f);

	glUniform1i(godrayShader.getUniformLocation("ColorBuffer"), 2);
	glUniform4f(godrayShader.getUniformLocation("SunPos"), sunPos.x, sunPos.y, sunPos.z, 0.0f);
	glUniformMatrix4fv(godrayShader.getUniformLocation("vp"), 1, GL_FALSE, &vp[0][0]);

//	std::cout << SunScreenPos.x << " " << SunScreenPos.y << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
	glVertex2f(0.0f, 0.0f);
	glVertex2f(1.0f, 0.0f);
	glVertex2f(1.0f, 1.0f);
	glVertex2f(0.0f, 1.0f);
	glEnd();
	
	godrayShader.unbind();
	SDL_GL_SwapWindow(window);
}

void processInputs(unsigned int delta)
{
	bool handledCam = false;
	SDL_Event eventHolder;
	while (SDL_PollEvent(&eventHolder)) {
		switch (eventHolder.type) {
		case SDL_QUIT:
			currentState = State::QUIT;
			break;

		case SDL_MOUSEMOTION:
			if (eventHolder.motion.x != 1024 / 2 && eventHolder.motion.y != 768 / 2) {
				//std::cout << "Real:" << eventHolder.motion.x << " " << eventHolder.motion.y << std::endl;
				//std::cout << "Relative:" << eventHolder.motion.xrel << " " << eventHolder.motion.yrel << std::endl;
				camera.computeMatricesFromInputs(delta, eventHolder.motion.xrel, eventHolder.motion.yrel);
				handledCam = true;
			}
			break;

		case SDL_KEYDOWN:
			if (eventHolder.key.keysym.sym == SDLK_ESCAPE) {
				currentState = State::QUIT;
			}
			if (eventHolder.key.keysym.sym == SDLK_r) {
				cloth.reset();
			}
			camera.keyDown(eventHolder.key.keysym);
			break;

		case SDL_MOUSEBUTTONDOWN:
			sphere.position = camera.pos + camera.direction * 3.0f;
			sphere.direction = camera.direction * 30.0f;

		case SDL_KEYUP:
			camera.keyUp(eventHolder.key.keysym);
			break;
		}
	}

	if (SDL_GetWindowFlags(window) & (SDL_WINDOW_INPUT_FOCUS)) {
		SDL_SetRelativeMouseMode(SDL_TRUE);
		//SDL_SetWindowGrab(window, SDL_TRUE);
		//SDL_ShowCursor(0);
		//SDL_WarpMouseInWindow(window, 1024 / 2, 768 / 2);
	}

	if (!handledCam) {
		camera.computeMatricesFromInputs(delta, 0, 0);
	}
}
