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

using namespace glm;
using namespace vogl;

bool g_hasMouse = false
   , g_cam_mode = false
   , g_cam_select = false;

Camera *g_cam = nullptr;
Lights *g_lights = Lights::getInstance();
uint g_spotlight, g_spotgeom;
vec3 g_spotlight_pos;
mat4 g_spotlight_rot;
float g_light_array[160];
GLint g_num_of_lights;

int g_width = 800, g_height = 600;

void error_callback( int error, const char * description )
{
	fputs( description, stderr );
}

void key_callback( GLFWwindow * window, int key, int scancode
                          , int action,	int mods )
{
	if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
	{ // exit captured mouse or exit program
		if ( g_hasMouse )
		{
			g_hasMouse = false;
			glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
		}
		else
		{
			glfwSetWindowShouldClose( window, GL_TRUE );
		}
	}
	else if ( key == GLFW_KEY_V && action == GLFW_PRESS )
	{
		if ( g_cam_mode )
			g_cam_mode = false;
		else
		{
			g_cam_mode = true;
			g_cam->setLookCenter();
		}
	}
	// directions of camera movement
	else if ( g_cam_mode )
	{
		if ( key == GLFW_KEY_W )
		{ // move forward
			if ( action == GLFW_PRESS )
				g_cam->walkOn( true );
			else if ( action == GLFW_RELEASE )
				g_cam->walkOff( true );
		}
		else if ( key == GLFW_KEY_S )
		{ // move backward
			if ( action == GLFW_PRESS )
				g_cam->walkOn( false );
			else if ( action == GLFW_RELEASE )
				g_cam->walkOff( false );
		}
		else if ( key == GLFW_KEY_A )
		{ // move left
			if ( action == GLFW_PRESS )
				g_cam->strafeOn( false );
			else if ( action == GLFW_RELEASE )
				g_cam->strafeOff( false );
		}
		else if ( key == GLFW_KEY_D )
		{ // move right
			if ( action == GLFW_PRESS )
				g_cam->strafeOn( true );
			else if ( action == GLFW_RELEASE )
				g_cam->strafeOff( true );
		}
	}
	else if ( key == GLFW_KEY_L && action == GLFW_PRESS )
	{
	  g_cam_select = !g_cam_select;
	}
	else if ( g_cam_select )
	{
	  if ( key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS )
		{
			g_lights->moveLight( vec3( 0.0f, 0.5f, 0.0f ), g_spotlight );
			g_lights->getLights( g_light_array, g_num_of_lights );
			g_spotlight_pos = vec3( g_lights->getPosition( g_spotlight ) );
		}
		else if ( key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS )
		{
			g_lights->moveLight( vec3( 0.0f, -0.5f, 0.0f ), g_spotlight );
			g_lights->getLights( g_light_array, g_num_of_lights );
			g_spotlight_pos = vec3( g_lights->getPosition( g_spotlight ) );
		}
		else if ( key == GLFW_KEY_UP && action == GLFW_PRESS )
		{
			g_lights->moveLight( vec3( 0.0f, 0.0f, 0.5f ), g_spotlight );
			g_lights->getLights( g_light_array, g_num_of_lights );
			g_spotlight_pos = vec3( g_lights->getPosition( g_spotlight ) );
		}
		else if ( key == GLFW_KEY_DOWN && action == GLFW_PRESS )
		{
			g_lights->moveLight( vec3( 0.0f, 0.0f, -0.5f ), g_spotlight );
			g_lights->getLights( g_light_array, g_num_of_lights );
			g_spotlight_pos = vec3( g_lights->getPosition( g_spotlight ) );
		}
		else if ( key == GLFW_KEY_RIGHT && action == GLFW_PRESS )
		{
			g_lights->moveLight( vec3( 0.5f, 0.0f, 0.0f ), g_spotlight );
			g_lights->getLights( g_light_array, g_num_of_lights );
			g_spotlight_pos = vec3( g_lights->getPosition( g_spotlight ) );
		}
		else if ( key == GLFW_KEY_LEFT && action == GLFW_PRESS )
		{
			g_lights->moveLight( vec3( -0.5f, 0.0f, 0.0f ), g_spotlight );
			g_lights->getLights( g_light_array, g_num_of_lights );
			g_spotlight_pos = vec3( g_lights->getPosition( g_spotlight ) );
		}
	}
}

void mousebutton_callback( GLFWwindow * window, int button
                                  , int action, int mods )
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
	  if ( g_cam_select )
	  {
	    float yr = ((g_height / 2) - y) * 0.002f;
	    float xr = ((g_width / 2) - x) * 0.002f;
	    if (x)
	    {
	      g_spotlight_rot = rotate( g_spotlight_rot, xr, vec3(1,0,0) );
	      g_lights->directLight( mat3(g_spotlight_rot), g_spotlight );
	    }
	    if (y)
	    {
	      g_spotlight_rot = rotate( g_spotlight_rot, yr, vec3(0,0,1) );
	      g_lights->directLight( mat3(g_spotlight_rot), g_spotlight );
	    }
	    glfwSetCursorPos( window, g_width / 2, g_height / 2 );
	    g_lights->getLights( g_light_array, g_num_of_lights );
	  }
	  else if ( g_cam_mode )
		{
			g_cam->rotateY( (float)((g_width / 2) - x) * 0.05f );
			g_cam->rotateX( (float)((g_height / 2) - y) * 0.05f );
			glfwSetCursorPos( window, g_width / 2, g_height / 2 );
		}
		else
		{
			g_cam->rotateOrigY( (float)((g_width / 2) - x) * 0.002f );
			g_cam->rotateOrigX( (float)((g_height / 2) - y) * 0.002f );
			glfwSetCursorPos( window, g_width / 2, g_height / 2 );
		}
	}
}

void scroll_callback( GLFWwindow * window, double x, double y )
{
  if ( y > 0 )
	{
    if ( g_cam_select )
    {
      g_lights->beamLight( 1.0, g_spotlight );
      g_lights->getLights( g_light_array, g_num_of_lights );
    }
    else
      // zoom in
      g_cam->zoomIn();

	}
	else if ( y < 0 )
	{
	  if ( g_cam_select )
	  {
	    g_lights->beamLight( -1.0, g_spotlight );
	    g_lights->getLights( g_light_array, g_num_of_lights );
	  }
	  else
      // zoom out
      g_cam->zoomOut();

	}
}

void resize_callback( GLFWwindow * window, int newWidth, int newHeight )
{
	g_width = newWidth;
	g_height = newHeight;
	g_height = g_height > 0 ? g_height : 1;
	g_cam->setAspectRatio( g_width, g_height );
	glViewport( 0, 0, g_width, g_height );
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
	for ( GLenum currError = glGetError(); currError != GL_NO_ERROR; currError =
			glGetError() )
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
//	glEnable( GL_CULL_FACE );
//	glCullFace( GL_BACK );
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
		shader.addUniform( "matAmb" );
		shader.addUniform( "matSpec" );
		shader.addUniform( "numLights" );
		shader.addUniform( "allLights[0]" );
		shader.addUniform("image");
		shader.addUniform("eMap");
	shader.unUse();
	// print debugging info
	shader.printActiveUniforms();
	// shader for drawing the lamp shade
	Shader widget;
	widget.loadFromFile( GL_VERTEX_SHADER, "vertex.simple.glsl" );
	widget.loadFromFile( GL_FRAGMENT_SHADER, "fragment.simple.glsl" );
	widget.createAndLinkProgram();
	widget.use();
		widget.addUniform( "mvM" );
		widget.addUniform( "projM" );
	widget.unUse();
	/****************************************************************************
	 * Setup Lighting
	 ***************************************************************************/
	g_spotlight_pos = vec3( 0.0f, 7.0f, 0.0f );
	Geometry *geo = Geometry::getInstance();
	uint texture1 = shader("eMap");
	uint texture0 = shader("image");
	g_spotgeom = geo->addBuffer( "lamp.obj", g_spotlight_pos, vec3( 0.7f, 0.7f, 0.7f ) );
	uint sphere = geo->addBuffer( "res/assets/sphere.obj"
	                            , vec3( -5.0f, 0.9f, 5.0f )
	                            , vec3( 0.714f, 0.4284f, 0.18144f ) );
	uint bunny = geo->addBuffer( "res/assets/bunny.obj"
	                            , vec3( 0.0f, -0.5f, 0.0f )
	                            , vec3( 0.50754f, 0.50754f, 0.50754f ) );
	uint box = geo->addBuffer( "res/assets/box.obj", vec3( 5.0f, 1.5f, -5.0f ) );
	uint torus = geo->addBuffer( "res/assets/torus.obj"
	                            , vec3( 5.0f, 0.0f, 5.0f )
	                            , vec3( 0.5f, 0.0f, 0.0f ) );
	uint teapot = geo->addBuffer( "res/assets/teapot.obj"
	                            , vec3( -5.0f, -0.5f, -5.0f )
	                            , vec3( 0.427451f, 0.470588f, 0.541176f ) );
	uint table = geo->addBuffer( "res/assets/table.obj", vec3( 0, -1, 0 ) );
	if ( checkGLErrors( 375 ) ) exit(1);
	geo->bindCMTexure( "res/textures/cubeMap.jpg", teapot );
	geo->bindTexure( "res/textures/wood.jpg", table );
	geo->bindTexure( "res/textures/brick.jpg", box );

	g_lights->addPointLight( vec3( 2.0f, 3.0f, 1.0f )
	                       , vec3( 0.7f, 0.7f, 0.7f )
	                       , 1.0f, 0.0f, 0.0f, 0.1f );
	g_spotlight = g_lights->addSpotLight( g_spotlight_pos
	                                    , vec3( 1.0f, 1.0f, 1.0f )
	                                    ,	1.0f, 0.0f, 0.0f, 0.1f
	                                    , vec3( 0.0f, -1.0f, 0.0f ), 45.0f );
	g_lights->addDirectionalLight( vec3( 0.0f, -1.0f, 0.0f )
	                             , vec3( 0.6f,  0.6f, 0.6f ) );
//  lights->addSpotLight( vec3( 0.0f, 10.0f, 0.0f ), vec3( 1.0f, 1.0f, 1.0f )
//                      , 1.0f, 0.0f, 0.0f, 0.05f, vec3( 0.0f, -1.0f, 0.0f ), 10.0f );
	g_lights->getLights( g_light_array, g_num_of_lights );

	// Camera to get model view and projection matices from. Amongst other things
	g_cam = new Camera( vec3( 0.0f, 2.0f, 25.0f ), g_width, g_height );
	g_cam->setLookCenter();
	float redplast[] = { 0.0f, 0.0f, 0.0f, 0.0f // ambient + reflect
	                   , 0.7f, 0.6f, 0.6f // specular
	                   , 0.25f }; // shininess
	float bronze[] = { 0.2125f, 0.1275f, 0.054f, 0.0f
	                , 0.393548f, 0.271906f, 0.166721f, 0.2f };
	float china[] = { 0.19225f, 0.19225f, 0.19225f, 0.0f
	                , 0.508273f, 0.508273f, 0.508273f, 0.2f };
	float bMetal[] = { 0.105882f, 0.058824f, 0.113725f, 0.5f
	                , 0.333333f, 0.333333f, 0.521569f, 0.84615f };
	float def[] 	= { 0.05f, 0.05f, 0.05f, 0.0f
		                , 1.0f, 1.0f, 1.0f, 4.0f };
	///////////////////////////////////////////////////////////////////////////
	//                           Main Rendering Loop                         //
	///////////////////////////////////////////////////////////////////////////
	float black[] =	{ 0, 0, 0 };
	glClearBufferfv( GL_COLOR, 0, black );
	glViewport( 0, 0, g_width, g_height );

//	Texture *txt = Texture::getInstance();
//	uint reflect = txt->setupEnvMap( "reflect", 512 );
//	geo->bindCMTexure( reflect, bunny );

	Shader *env;
	while ( !glfwWindowShouldClose( window ) )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// load values into the uniform slots of the shader and draw
//		txt->useEnvironmentMap( env, vec3( 0.0f, 0.5f, 0.0f ), "reflect" );
//			glUniform1i( (*env)( "image" ), 0 );
//			checkGLErrors( 424 );
//			glUniform1i( (*env)( "numLights" ), g_num_of_lights );
//			checkGLErrors( 426 );
//			glUniformMatrix4fv( (*env)( "allLights[0]" ), g_num_of_lights
//					, GL_FALSE, g_light_array );
//			checkGLErrors( 429 );
//			glUniform4fv( (*env)( "matAmb" ), 1, &bronze[0] );
//			glUniform4fv( (*env)( "matSpec" ), 1, &bronze[4] );
//			geo->draw( sphere, 1 );
////			glUniform4fv( (*env)( "matAmb" ), 1, &china[0] );
////			glUniform4fv( (*env)( "matSpec" ), 1, &china[4] );
////			geo->draw( bunny, 1 );
//			glUniform4fv( (*env)( "matAmb" ), 1, &redplast[0] );
//			glUniform4fv( (*env)( "matSpec" ), 1, &redplast[4] );
//			geo->draw( torus, 1 );
//			glUniform4fv( (*env)( "matAmb" ), 1, &bMetal[0] );
//			glUniform4fv( (*env)( "matSpec" ), 1, &bMetal[4] );
//			geo->draw( teapot, 1 );
//			glUniform4fv( (*env)( "matAmb" ), 1, &def[0] );
//			glUniform4fv( (*env)( "matSpec" ), 1, &def[4] );
//			geo->draw( box, 1 );
//			geo->draw( table, 1 );
//		txt->unUseEnvironmentMap( g_width, g_height, "reflect" );

		shader.use();
			glUniform1i( texture0, 0 );
			glUniform1i( texture1, 1 );
			glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE,
					value_ptr( g_cam->getViewMatrix() ) );
			glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,
					value_ptr( g_cam->getProjectionMatrix() ) );
			glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,
					value_ptr( g_cam->getNormalMatrix() ) );
			glUniform1i( shader( "numLights" ), g_num_of_lights );
			glUniformMatrix4fv( shader( "allLights[0]" ), g_num_of_lights
					, GL_FALSE, g_light_array );
			glUniform4fv( shader( "matAmb" ), 1, &bronze[0] );
			glUniform4fv( shader( "matSpec" ), 1, &bronze[4] );
			geo->draw( sphere, 1 );
			glUniform4fv( shader( "matAmb" ), 1, &china[0] );
			glUniform4fv( shader( "matSpec" ), 1, &china[4] );
			geo->draw( bunny, 1 );
			glUniform4fv( shader( "matAmb" ), 1, &redplast[0] );
			glUniform4fv( shader( "matSpec" ), 1, &redplast[4] );
			geo->draw( torus, 1 );
			glUniform4fv( shader( "matAmb" ), 1, &bMetal[0] );
			glUniform4fv( shader( "matSpec" ), 1, &bMetal[4] );
			geo->draw( teapot, 1 );
			glUniform4fv( shader( "matAmb" ), 1, &def[0] );
			glUniform4fv( shader( "matSpec" ), 1, &def[4] );
			geo->draw( box, 1 );
			geo->draw( table, 1 );
		shader.unUse();

		widget.use();
		  glUniformMatrix4fv( widget( "mvM" ), 1, GL_FALSE, value_ptr(
				  translate( g_cam->getViewMatrix(), g_spotlight_pos ) * g_spotlight_rot ) );
		  glUniformMatrix4fv( widget( "projM" ), 1, GL_FALSE
				  , value_ptr( g_cam->getProjectionMatrix() ) );
		  geo->draw( g_spotgeom, 1 );
		widget.unUse();

		// make sure the camera rotations, position and matrices are updated
		g_cam->update();

		glfwSwapBuffers( window );

		glfwPollEvents();
	}

	delete (g_cam);
	delete (g_lights);

	glfwTerminate();

	return (0);
}
