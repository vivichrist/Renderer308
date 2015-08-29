#define GLM_FORCE_RADIANS

#define WIDTH 800
#define HEIGHT 600

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>   // file I/O
#include <exception>
#include "Camera.hpp"
#include "Geometry.hpp"
#include "Shader.hpp"

using namespace glm;

bool g_hasMouse = false;
vogl::Camera *g_cam = nullptr;
int g_width = 800, g_height = 600;

void error_callback( int error, const char * description )
{
	fputs( description, stderr );
}

void key_callback( GLFWwindow * window, int key, int scancode, int action,
		int mods )
{
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	{// exit captured mouse or exit program
		if ( g_hasMouse )
		{
			g_hasMouse = false;
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
		}
		else
		{
			glfwSetWindowShouldClose( window, GL_TRUE );
			delete (g_cam);
		}
	}
	// directions of camera movement
	else if ( key == GLFW_KEY_W )
	{// move forward
		if ( action == GLFW_PRESS )
			g_cam->walkOn( true );
		else if ( action == GLFW_RELEASE )
			g_cam->walkOff( true );
	}
	else if ( key == GLFW_KEY_S )
	{// move backward
		if ( action == GLFW_PRESS )
			g_cam->walkOn( false );
		else if ( action == GLFW_RELEASE )
			g_cam->walkOff( false );
	}
	else if ( key == GLFW_KEY_A )
	{// move left
		if ( action == GLFW_PRESS )
			g_cam->strafeOn( false );
		else if ( action == GLFW_RELEASE )
			g_cam->strafeOff( false );
	}
	else if ( key == GLFW_KEY_D )
	{// move right
		if ( action == GLFW_PRESS )
			g_cam->strafeOn( true );
		else if ( action == GLFW_RELEASE )
			g_cam->strafeOff( true );
	}
}

void mousebutton_callback( GLFWwindow * window, int button, int action,
		int mods )
{
	if ( button == GLFW_MOUSE_BUTTON_1 )
	{
		if ( g_hasMouse )
		{
			// TODO: do some such thing with mouse click...
		}
		else
		{ // capture mouse and allow mouse to rotate camera view
			g_hasMouse = true;
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
			glfwSetCursorPos( window, g_width / 2, g_height / 2 );
		}
	}
}

void mousemotion_callback( GLFWwindow * window, double x, double y )
{
	if ( g_hasMouse )
	{ // apply rotations of the capured mouse cursor
		g_cam->rotateY( ((g_width / 2) - x) * 0.05f );
		g_cam->rotateX( ((g_height / 2) - y) * 0.05f );
		glfwSetCursorPos( window, g_width / 2, g_height / 2 );
	}
}

void scroll_callback( GLFWwindow * window, double x, double y )
{
  // TODO: this could be useful later...
}

void resize_callback( GLFWwindow * window, int newWidth, int newHeight )
{
	g_width = newWidth;
	g_height = newHeight > 0 ? newHeight : 1;
	glViewport( 0, 0, newWidth, newHeight );
}

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
	///////////////////////////////////////////////////////////////////////////
	// Initiate the opengl context and try to get an opengl 3.3 context. Then//
	// initialise the scene.                                                 //
	///////////////////////////////////////////////////////////////////////////
	if ( !glfwInit() )
		exit( EXIT_FAILURE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );

	GLFWwindow * window = glfwCreateWindow( g_width, g_height,
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
		if ( glewIsSupported( "GL_VERSION_3_3" ) )
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

	glfwSetErrorCallback( error_callback );
	glfwSetKeyCallback( window, key_callback );
	glfwSetScrollCallback( window, scroll_callback );
	glfwSetMouseButtonCallback( window, mousebutton_callback );
	glfwSetCursorPosCallback( window, mousemotion_callback );
	glfwSetFramebufferSizeCallback( window, resize_callback );
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );

	vogl::Geometry geo;
	uint name = geo.addBuffer( "teapot.obj" );
	geo.bindTexure( "box.png", name );

	/************************************************************
	 * Load up a shader from the given files.
	 *******************************************************//**/
	Shader shader;
	shader.loadFromFile( GL_VERTEX_SHADER, "vertex_phong.glsl" );
	shader.loadFromFile( GL_FRAGMENT_SHADER, "fragment_phong.glsl" );
	shader.createAndLinkProgram();
	shader.use();
		shader.addUniform( "mvM" );
		shader.addUniform( "projM" );
		shader.addUniform( "normM" );
		shader.addUniform( "lightP" );
	shader.unUse();
	// print debuging info
	shader.printActiveUniforms();
	// Camera to get model view and projection matices from. Amongst other things
	g_cam = new vogl::Camera( vec3( 0.0f, 0.0f, 10.0f ), g_width, g_height );

	float black[] =	{ 0, 0, 0 };
	glClearBufferfv( GL_COLOR, 0, black );
	float lightP[] = { 5.0f, 5.0f, 100.0f };
	///////////////////////////////////////////////////////////////////////////
	//                           Main Rendering Loop                         //
	///////////////////////////////////////////////////////////////////////////
	while ( !glfwWindowShouldClose( window ) )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// load values into the uniform slots of the shader and draw
		shader.use();
			glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE,
					value_ptr( g_cam->getViewMatrix() ) );
			glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,
					value_ptr( g_cam->getProjectionMatrix() ) );
			glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,
					value_ptr( g_cam->getNormalMatrix() ) );
			glUniform3fv( shader( "lightP" ), 1, &lightP[0] );
			geo.draw( name, 1 );
		shader.unUse();
		// make sure the camera rotations, position and matrices are updated
		g_cam->update();

		glfwSwapBuffers( window );

		glfwPollEvents();
	}
	delete (g_cam);

	glfwTerminate();

	return (0);
}
