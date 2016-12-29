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

	GLFWwindow* window;
	bool mouseLook;
	int width, height;

public:
	Engine( uint major, uint minor );
	static int gLErrors( int where, const char* className );

	Camera *cam;
	Lights *lights;
	Geometry *geo;
	Texture *txt;

	virtual void error( int error, const char* description ) = 0;
	virtual void resize( GLFWwindow * window, int newWidth, int newHeight );
	virtual void keyboardEvent( GLFWwindow * window, int key, int scancode
								, int action, int mods ) = 0;
	virtual void mouseScrollEvent( GLFWwindow * window, double x, double y ) = 0;
	virtual void mouseButtonEvent( GLFWwindow * window, int button, int action, int mods ) = 0;
	virtual void mouseMoveEvent( GLFWwindow * window, double x, double y ) = 0;

	virtual void update() = 0;
	virtual void render() = 0;
	virtual void resize() = 0;

	void captureMouse();
	void resetMousePos();
	void releaseMouse();
	void close();

	virtual ~Engine();

	/**< @class Engine */
};

Engine *eng = nullptr;

} /**< namespace Engine */

#endif /* SRC_ENGINE_H_ */
