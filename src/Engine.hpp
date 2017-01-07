/**************************************************************************//**
 * @class Engine
 * @file Engine.h
 * @brief ...
 *  Created on: 10/01/2016
 *      Author: vivichrist
 *****************************************************************************/

#pragma once

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>   // file I/O
#include <exception>
#include <memory>
#include "Camera.hpp"
#include "ShaderManager.hpp"
#include "Geometry.hpp"
#include "Lights.hpp"

using namespace R308;
namespace Engine
{


class Engine
{
protected:
	Lights *lights;
	Geometry *geo;
	Texture *txt;
	ShaderManager *sm;
	int width, height, halfWidth, halfHeight;
	Camera cam;
	bool mouseLook, hasMouse;
	GLFWwindow* window;
	Engine( uint major, uint minor, int width, int height );

public:
	static Engine *getInstance();
	static int gLErrors(int where, const char *className);
	virtual void errorCallback(int error, const char *description);
	virtual void keyboardEvent(int& key, int& scancode, int& action, int& mods);
	virtual void mouseScrollCallback(double& x, double& y);
	virtual void mouseButtonEvent(int& button, int& action, int& mods);
	virtual void mouseMovementEvent(double& x, double& y);
	virtual void windowResizeEvent(int& newWidth, int& newHeight);

void start();
virtual void init();
virtual void update();
virtual void render();

void captureMouse();
void resetMousePos();
void releaseMouse();
int checkGLErrors(int where, const char *className);
void close();

virtual ~Engine();

/**< @class Engine */
};


} /**< namespace Engine */
