#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>   // file I/O
#include <exception>
#include "Engine.hpp"
#include "Camera.hpp"
#include "Geometry.hpp"
#include "Shader.hpp"
#include "UniformBlock.hpp"

using namespace glm;

/******************************************************************************
 * registered opengl errors can be reported at any point where this function is
 * called.
 * @param where
 * @return
 */
int checkGLErrors( int where )
{
	int errCount = 0;
	for ( GLenum currError = glGetError();
				 currError != GL_NO_ERROR;
				 currError = glGetError() )
	{
		cout << "Error: " << currError << " line " << where << " In Main\n";
		++errCount;
	}

	return errCount;
}

int main()
{
	Engine::Engine* eng = Engine::Engine::getInstance();
    eng->start();
	
    return (0);
}