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
#include "gbuffer.hpp"

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

GBuffer buff;

GLuint depthTexture;
GLuint depthTex;
GLuint depthBuffer;

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

	// buff.Init(g_width, g_height);

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
	Shader env;
  env.loadFromFile( GL_VERTEX_SHADER, "vertex_deferred.glsl" );
  env.loadFromFile( GL_FRAGMENT_SHADER, "fragment_deferred.glsl" );
  env.createAndLinkProgram();
  env.use();
    env.addUniform( "mvM" ); checkGLErrors( 340 );
    env.addUniform( "projM" ); checkGLErrors( 341 );
    env.addUniform( "normM" ); checkGLErrors( 342 );
    env.addUniform( "matNorm" ); checkGLErrors( 343 );
    env.addUniform( "image" ); checkGLErrors( 344 );
    env.addUniform( "normalMap" ); checkGLErrors( 343 );
  env.unUse();
  // print debugging info
  env.printActiveUniforms();

	Shader shader;
	shader.loadFromFile( GL_VERTEX_SHADER, "vertex_postdef.glsl" );
	shader.loadFromFile( GL_GEOMETRY_SHADER, "geometry_postdef.glsl" );
	shader.loadFromFile( GL_FRAGMENT_SHADER, "fragment_postdef2.glsl" );
	shader.createAndLinkProgram();
	shader.use();
		shader.addUniform( "mvM" ); checkGLErrors( 356 );
		shader.addUniform( "normM" ); checkGLErrors( 357 );
		shader.addUniform( "projM" ); checkGLErrors( 358 );
		shader.addUniform( "numLights" ); checkGLErrors( 359 );
		shader.addUniform( "allLights[0]" ); checkGLErrors( 360 );
    shader.addUniform( "depth" ); checkGLErrors( 360 );
    shader.addUniform( "colour" ); checkGLErrors( 361 );
    shader.addUniform( "normals" ); checkGLErrors( 362 );
    shader.addUniform( "eyepos" ); checkGLErrors( 363 );
    //shader.addUniform( "reflects" );
	shader.unUse();
	// print debugging info
	shader.printActiveUniforms();

	/****************************************************************************
	 * Setup Geometry
	 ***************************************************************************/
	g_spotlight_pos = vec3( 0.0f, 7.0f, 0.0f );
	Geometry *geo = Geometry::getInstance();
	// g_spotgeom = geo->addBuffer( "lamp.obj", g_spotlight_pos, vec3( 0.7f, 0.7f, 0.7f ) );
	uint bunny = geo->addBuffer( "res/assets/bunny.obj"
	                            , vec3( 0.0f, -0.5f, 0.0f ) );

	if ( checkGLErrors( 375 ) ) exit(1);
  Texture *txt = Texture::getInstance();
	geo->bindTexure( "res/textures/brick.jpg", bunny );
	geo->bindNMTexure( "res/textures/brick_normal.jpg", bunny );

	/****************************************************************************
	 * Setup Lighting
	 ***************************************************************************/
	g_lights->addPointLight( vec3( 5.0f, 5.0f, 5.0f )
	                       , vec3( 1.7f, 1.7f, 1.7f )
	                       , 1.0f, 0.0f, 0.0f, 0.1f );

	g_lights->getLights( g_light_array, g_num_of_lights );

	// Camera to get model view and projection matices from. Amongst other things
	g_cam = new Camera( vec3( 0.0f, 2.0f, 25.0f ), g_width, g_height );
	g_cam->setLookCenter();

	// GLfloat defCube = 0.0f;
	GLfloat defNormal = 0.5f;
	uint fbo = txt->setupFBO( g_width, g_height );
	///////////////////////////////////////////////////////////////////////////
	//                           Main Rendering Loop                         //
	///////////////////////////////////////////////////////////////////////////
	float black[] =	{ 0, 0, 0 };
	glClearBufferfv( GL_COLOR, 0, black );
	glViewport( 0, 0, g_width, g_height );

	while ( !glfwWindowShouldClose( window ) )
	{
	  // txt->activateFrameBuffer( fbo ); checkGLErrors( 409 );
		// load values into the uniform slots of the shader and draw
		env.use(); checkGLErrors( 411 );
			glUniform1i( env( "image" ), 0 );
			checkGLErrors( 418 );
			glUniform1i( env( "normalMap" ), 1 );
			checkGLErrors( 420 );
			glUniformMatrix4fv( env( "mvM" ), 1, GL_FALSE, value_ptr( g_cam->getViewMatrix() ) );
			checkGLErrors( 422 );
			glUniformMatrix4fv( env( "projM" ), 1, GL_FALSE, value_ptr( g_cam->getProjectionMatrix() ) );
			checkGLErrors( 424 );
			glUniformMatrix3fv( env( "normM" ), 1, GL_FALSE, value_ptr( g_cam->getNormalMatrix() ) );
			checkGLErrors( 426 );
			glUniform1f( env( "matNorm" ), defNormal );
      geo->draw( bunny, 1 );
      checkGLErrors( 428 );
		env.unUse();

		// txt->activateTexturesFB( fbo );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
			glUniform1i( shader("depth"), 0 );
			checkGLErrors( 435 );
			glUniform1i( shader("colour"), 1 );
			checkGLErrors( 437 );
			glUniform1i( shader("normals"), 2 );
			checkGLErrors( 439 );
			glUniform1i( shader("eyepos"), 3 );
			checkGLErrors( 441 );
			// glUniform1i( shader("reflects"), 4 );
			glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE, value_ptr( g_cam->getViewMatrix() ) );
			checkGLErrors( 444 );
			glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,	value_ptr( g_cam->getNormalMatrix() ) );
			checkGLErrors( 446 );
			glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE, value_ptr( g_cam->getProjectionMatrix() ) );
			checkGLErrors( 448 );
			glUniformMatrix4fv( shader( "allLights[0]" ), g_num_of_lights, GL_FALSE, g_light_array );
			checkGLErrors( 450 );
			glUniform1i( shader( "numLights" ), g_num_of_lights );
			checkGLErrors( 452 );
		shader.unUse();

		// make sure the camera rotations, position and matrices are updated
		g_cam->update();

		glfwSwapBuffers( window );

		glfwPollEvents();
	}

	delete (geo);
	delete (txt);
	delete (g_cam);
	delete (g_lights);

	glfwTerminate();

	return (0);
}
