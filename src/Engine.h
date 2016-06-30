/**************************************************************************//**
 * @class Engine
 * @file Engine.h
 * @brief ...
 *  Created on: 10/01/2016
 *      Author: vivichrist
 *****************************************************************************/

#ifndef SRC_ENGINE_H_
#define SRC_ENGINE_H_

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
	static void resizeCallback( GLFWwindow * window
								, int newWidth, int newHeight );
	static void errorCallback( int error, const char* description );
	static void mousemotion_callback( GLFWwindow * window
									   , double x, double y );
	static void mousebutton_callback( GLFWwindow * window
									   , int button, int action, int mods );
	static void key_callback( GLFWwindow * window
							  , int key, int scancode, int action, int mods );
	static void scroll_callback( GLFWwindow * window, double x, double y );

	GLFWwindow* window;
	bool mouseLook;
	int width, height;

public:
	Engine( uint major, uint minor );
	static int checkGLErrors( int where, const char* className );

	Camera *cam;
	Lights *lights;
	Geometry *geo;
	Texture *txt;

	virtual void keyboardEvent( int key, int scancode
								, int action, int mods );
	virtual void mouseScrollEvent( double x, double y );
	virtual void mouseButtonEvent( int button, int action, int mods );
	virtual void mouseMoveEvent( double x, double y );

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
