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
Engine *instance = nullptr;

Engine* Engine::getInstance() // Singleton is accessed via getInstance()
{
	if (!instance)
	{
		instance = new Engine(4, 3, 1024, 768);
	}    
	return instance;
}

Engine::Engine( uint major, uint minor, int w, int h )
	: lights(Lights::getInstance()), geo(Geometry::getInstance()), txt(Texture::getInstance())
	, sm(ShaderManager::getInstance()), width(w), height(h), halfWidth(w / 2), halfHeight(h / 2)
	, cam(glm::vec3(0.0f, 0.0f, 10.0f), width, height), mouseLook(false), hasMouse(false)
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

	glfwSetWindowUserPointer(window, instance);
	
	//glfwSetErrorCallback( genericCallback(errorCallback) );
	auto key = [](GLFWwindow *w, int k, int s, int a, int m) {
	    static_cast<Engine *>(glfwGetWindowUserPointer(w))->keyboardEvent(k, s, a, m);
	};
	glfwSetKeyCallback(window, key);
	auto scroll = [](GLFWwindow *w, double x, double y) {
	    static_cast<Engine *>(glfwGetWindowUserPointer(w))->mouseScrollCallback(x, y);
	};
	glfwSetScrollCallback(window, scroll);
	auto move = [](GLFWwindow *w, double x, double y) {
	    static_cast<Engine *>(glfwGetWindowUserPointer(w))->mouseMovementEvent(x, y);
	};
	glfwSetCursorPosCallback(window, move);
	auto button = [](GLFWwindow *w, int b, int a, int m) {
	    static_cast<Engine *>(glfwGetWindowUserPointer(w))->mouseButtonEvent( b, a, m );
	};
	glfwSetMouseButtonCallback(window, button);
	auto resize = [](GLFWwindow *w, int wd, int h) {
	    static_cast<Engine *>(glfwGetWindowUserPointer(w))->windowResizeEvent( wd, h );
	};
	glfwSetFramebufferSizeCallback(window, resize);
}

void Engine::init()
{

	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	///////////////////////////////////////////////////////////////////////////
	// Define Scene															 //
	///////////////////////////////////////////////////////////////////////////
	uint name = geo->addBuffer( "res/assets/box.obj" );
	geo->bindTexure( "res/textures/box.png", name );

	/************************************************************
	 * Load up a shader from the given files.
	 *******************************************************//**/
	uint shaderID = sm->addShader("phong", name); // "phong.vert", "phong.frag" etc.
	sm->addUniform(name, "lightP", 3u, 1u);
	// Camera to get model view and projection matices from. Amongst other things
	R308::UniformBlock ubo( 1, 11 );
	ubo.bindUniformBlock( shaderID, "Cam" );
	cam.registerUBO( &ubo );

	float black[] = {0, 0, 0};
	glClearBufferfv( GL_COLOR, 0, black );
}

void Engine::errorCallback( int error, const char* description )
{
	fputs( description, stderr );
}

void Engine::keyboardEvent( int& key, int& scancode, int& action, int& mods )
{
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	{// exit captured mouse or exit program
		if ( hasMouse )
		{
			hasMouse = false;
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
		}
		else
		{
			glfwSetWindowShouldClose( window, GL_TRUE );
		}
	}
	// directions of camera movement
	else if ( key == GLFW_KEY_W )
	{// move forward
		if ( action == GLFW_PRESS )
			cam.walkOn( true );
		else if ( action == GLFW_RELEASE )
			cam.walkOff( true );
	}
	else if ( key == GLFW_KEY_S )
	{// move backward
		if ( action == GLFW_PRESS )
			cam.walkOn( false );
		else if ( action == GLFW_RELEASE )
			cam.walkOff( false );
	}
	else if ( key == GLFW_KEY_A )
	{// move left
		if ( action == GLFW_PRESS )
			cam.strafeOn( false );
		else if ( action == GLFW_RELEASE )
			cam.strafeOff( false );
	}
	else if ( key == GLFW_KEY_D )
	{// move right
		if ( action == GLFW_PRESS )
			cam.strafeOn( true );
		else if ( action == GLFW_RELEASE )
			cam.strafeOff( true );
	}
}

void Engine::mouseScrollCallback( double& x, double& y)
{
	// TODO: this could be useful later...
}

void Engine::mouseMovementEvent( double& x, double& y) {
	if (hasMouse)
	{ // apply rotations of the capured mouse cursor
	cam.rotateY((halfWidth - x) * 0.05f);
	cam.rotateX((halfHeight - y) * 0.05f);
	glfwSetCursorPos(window, halfWidth, halfHeight);
	}
}

void Engine::mouseButtonEvent( int& button, int& action, int& mods) {
	if (button == GLFW_MOUSE_BUTTON_1)
	{
		if (hasMouse)
		{
			// TODO: do some such thing with mouse click...
		}
		else
		{ // capture mouse and allow mouse to rotate camera view
			hasMouse = true;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, halfWidth, halfHeight);
		}
	}
}

void Engine::windowResizeEvent( int& newWidth, int& newHeight )
{
	width = newWidth;
	height = newHeight > 0 ? newHeight : 1;
	halfWidth = width / 2;
	halfHeight = height / 2;
	glViewport( 0, 0, width, height );
}

void Engine::update()
{

}

void Engine::render()
{
	// shader.use();
	// 	glUniform3fv( shader( "lightP" ), 1, &lightP[0] );
	// 	geo->draw( name, 1 );
	// shader.unUse();
}

void Engine::start()
{
	init();
	double start = glfwGetTime(), end;
	while (!glfwWindowShouldClose(window))
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// load values into the uniform slots of the shader and draw, etc.
		render();

		update();
		// make sure the camera rotations, position and matrices are updated, etc.
		cam.update(float((end = glfwGetTime()) - start));
		start = end;

		glfwSwapBuffers( window );
		glfwSwapInterval(1);
		glfwPollEvents();
	}
	update();
}

int checkGLErrors( int where, const char* className )
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
	delete (lights);

	glfwTerminate();
}

} /**< namespace Engine */
