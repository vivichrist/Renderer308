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
#include "Camera.hpp"
#include "Geometry.hpp"
#include "Lights.hpp"
#include "Shader.hpp"
#include "GBuffer.hpp"

using namespace R308;
namespace Engine
{

class Engine
{
private:
	// GLFW callback

	bool mouseLook;

public:
	Engine( uint major, uint minor );
	static int gLErrors( int where, const char* className );

	void setErrorCB( void(*errorCallback)(int error, const char* description) );
	void setKBEvtCB( void(*keyCallback)(GLFWwindow * window, int key, int scancode, int action, int mods ) );
	void setMScrollEvtCB( void(*mScrollCallback)(GLFWwindow * window, double x, double y ) );
	void setMButtonEvtCB( void(*mButtonCallback)(GLFWwindow * window, int button, int action, int mods ) );
	void setMMoveEvtCB( void(*mMoveCallback)( GLFWwindow * window, double x, double y ) );
	void setResizeCB( void(*resizeCallback)( GLFWwindow * window, int newWidth, int newHeight ) );
	
	virtual void update() = 0;
	virtual void render() = 0;

	void captureMouse();
	void resetMousePos();
	void releaseMouse();
	int checkGLErrors(int where, const char *className);
	void close();

	virtual ~Engine();

	/**< @class Engine */
};

Engine *eng = nullptr;

} /**< namespace Engine */
