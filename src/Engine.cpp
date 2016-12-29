/**************************************************************************//**
 * @class Engine
 * @file Engine.cpp
 * @brief ...
 *  Created on: 10/01/2016
 *      Author: vivichrist
 *****************************************************************************/

#include "Engine.hpp"

namespace Engine
{
////////////////////////////////////////////////////////////////////////////////
/// Methods to be overridden and interface with GLFW
////////////////////////////////////////////////////////////////////////////////

void Engine::resize( GLFWwindow * window, int newWidth, int newHeight )
{
	if ( eng == nullptr || window != eng->window ) return;
	eng->width = newWidth;
	eng->height = newHeight > 0 ? newHeight : 1;
	eng->cam->setAspectRatio( newWidth, newHeight );
	glViewport( 0, 0, newWidth, newHeight );
	eng->resize();
}

Engine::Engine( uint major, uint minor ) : width(1024), height(768)
{
	///////////////////////////////////////////////////////////////////////////
	// Initiate the opengl context and try to get an opengl context. Then    //
	// initialise the scene.                                                 //
	///////////////////////////////////////////////////////////////////////////
	if ( !glfwInit() )
		exit( EXIT_FAILURE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, major );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, minor );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );

	window = glfwCreateWindow( width, height,
			"GLFW Application", nullptr, nullptr );
	if ( !window )
	{
		glfwTerminate();
		std::cerr << "Could not create window";
		exit( EXIT_FAILURE );
	}

	glfwMakeContextCurrent( window );

	std::cout << "\nOpenGL Version: "
			<< glfwGetWindowAttrib( window, GLFW_CONTEXT_VERSION_MAJOR ) << "."
			<< glfwGetWindowAttrib( window, GLFW_CONTEXT_VERSION_MINOR ) << "."
			<< glfwGetWindowAttrib( window, GLFW_CONTEXT_REVISION ) << "\n";

// start GLEW extension handler
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	glGetError(); // flush an non existent error
	if ( GLEW_OK | err )
	{
		std::cerr << "Error: " << glewGetErrorString( err ) << std::endl;
	}
	else
	{
		if ( glewIsSupported( "GL_VERSION_4_5" ) )
		{
			std::cout << "Driver supports OpenGL 4.5\nDetails:" << std::endl;
		}
		else if ( glewIsSupported( "GL_VERSION_4_3" ) )
		{
			std::cout << "Driver supports OpenGL 4.3\nDetails:" << std::endl;
		}
		else if ( glewIsSupported( "GL_VERSION_4_0" ) )
		{
			std::cout << "Driver supports OpenGL 4.0\nDetails:" << std::endl;
		}
		else if ( glewIsSupported( "GL_VERSION_3_3" ) )
		{
			std::cout << "Driver supports OpenGL 3.3\nDetails:" << std::endl;
		}
		else if ( glewIsSupported( "GL_VERSION_3_2" ) )
		{
			std::cout << "Driver supports OpenGL 3.2\nDetails:" << std::endl;
		}
		else if ( glewIsSupported( "GL_VERSION_3_1" ) )
		{
			std::cout << "Driver supports OpenGL 3.1\nDetails:" << std::endl;
		}
	}
	std::cout << "\tUsing glew " << glewGetString( GLEW_VERSION ) << std::endl;
	std::cout << "\tVendor: " << glGetString( GL_VENDOR ) << std::endl;
	std::cout << "\tRenderer: " << glGetString( GL_RENDERER ) << std::endl;
	std::cout << "\tVersion: " << glGetString( GL_VERSION ) << std::endl;
	std::cout << "\tGLSL:	" << glGetString( GL_SHADING_LANGUAGE_VERSION )
			<< std::endl;
	glfwSwapInterval( 2 );

	glfwSetErrorCallback( errorCallback );
	glfwSetKeyCallback( window, key_callback );
	glfwSetScrollCallback( window, scroll_callback );
	glfwSetMouseButtonCallback( window, mousebutton_callback );
	glfwSetCursorPosCallback( window, mousemotion_callback );
	glfwSetFramebufferSizeCallback( window, resizeCallback );

	mouseLook = false;
	geo = Geometry::getInstance();
	txt = Texture::getInstance();
	cam = new Camera( glm::vec3( 0.0f, 2.0f, 10.0f ), width, height );
	lights = Lights::getInstance();
	eng = this;
}

int Engine::checkGLErrors( int where, const char* className )
{
	int errCount = 0;
	for ( GLenum currError = glGetError(); currError != GL_NO_ERROR;
			currError = glGetError() )
	{
		cout << "Error: " << currError << " line " << where << " In "
			 << className << "\n";
		++errCount;
	}

	return errCount;
}
////////////////////////////////////////////////////////////////////////////////
/// GLFW Mouse Manipulation
////////////////////////////////////////////////////////////////////////////////
void Engine::captureMouse()
{
	mouseLook = true;
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetCursorPos( window, width / 2, height / 2 );
}

void Engine::resetMousePos()
{
	glfwSetCursorPos( window, width / 2, height / 2 );
}

void Engine::releaseMouse()
{
	mouseLook = false;
	glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
}

void Engine::close()
{
	glfwSetWindowShouldClose( window, GL_TRUE );
}

Engine::~Engine()
{
	delete (geo);
	delete (txt);
	delete (cam);
	delete (lights);

	glfwTerminate();
}

} /**< namespace Engine */
