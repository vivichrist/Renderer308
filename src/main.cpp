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
#include "GeoObject.hpp"

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

	buff.Init(g_width, g_height);

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
  env.loadFromFile( GL_VERTEX_SHADER, "vertex_cmap.glsl" );
  env.loadFromFile( GL_GEOMETRY_SHADER, "geometry_cmap.glsl" );
  env.loadFromFile( GL_FRAGMENT_SHADER, "fragment_cmap.glsl" );
  env.createAndLinkProgram();
  env.use();
    env.addUniform( "mvM[0]" );
    env.addUniform( "projM" );
    env.addUniform( "normM[0]" );
    env.addUniform( "matAmb" );
    env.addUniform( "matSpec" );
    env.addUniform( "numLights" );
    env.addUniform( "allLights[0]" );
    env.addUniform("image");
  env.unUse();
  // print debugging info
  env.printActiveUniforms();

	Shader shader;
	shader.loadFromFile( GL_VERTEX_SHADER, "vertex_phong.glsl" );
	shader.loadFromFile( GL_FRAGMENT_SHADER, "fragment_phong.glsl" );
	shader.createAndLinkProgram();
	shader.use();
		shader.addUniform( "mvM" );
		shader.addUniform( "projM" );
		shader.addUniform( "normM" );
		shader.addUniform( "matCubemap" );
		shader.addUniform( "matNormal" );
		shader.addUniform( "matAmb" );
		shader.addUniform( "matSpec" );
		shader.addUniform( "numLights" );
		shader.addUniform( "allLights[0]" );
		shader.addUniform( "image" );
		shader.addUniform( "eMap" );
		shader.addUniform( "normalmap" );
		// shader.addUniform( "DepthTexture" );
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
	 * Setup Geometry
	 ***************************************************************************/
	g_spotlight_pos = vec3( 0.0f, 7.0f, 0.0f );
	Geometry *geo = Geometry::getInstance();
	g_spotgeom = geo->addBuffer( "lamp.obj", g_spotlight_pos,
			vec3( 0.7f, 0.7f, 0.7f ) );
	vector<GeoObject> objs;

	objs.push_back( GeoObject( "res/assets/sphere.obj", vec3( -5.0f, 0.9f, 5.0f )
			, vec3( 0.714f, 0.4284f, 0.18144f ) ) );
	objs.push_back( GeoObject( "res/assets/bunny.obj", vec3( 0.0f, -0.5f, 0.0f )
			, vec3( 0.50754f, 0.50754f, 0.50754f ) ) );
	objs.push_back( GeoObject( "res/assets/box.obj", vec3( 5.0f, 1.5f, -5.0f ) ) );
	objs.push_back( GeoObject( "res/assets/torus.obj", vec3( 5.0f, 0.0f, 5.0f )
			, vec3( 0.5f, 0.0f, 0.0f ) ) );
	objs.push_back( GeoObject( "res/assets/teapot.obj", vec3( -5.0f, -0.5f, -5.0f )
			, vec3( 0.427451f, 0.470588f, 0.541176f ) ) );
	objs.push_back( GeoObject( "res/assets/table.obj", vec3( 0, -1, 0 ) ) );
	if ( checkGLErrors( 375 ) )
		exit( 1 );
	objs[2].reflect( 512, GL_TEXTURE1 );
	objs[3].texture( "res/textures/normalMap.jpg", GL_NORMAL_MAP, GL_TEXTURE2 );
	objs[0].texture( "res/textures/brick2_normal.jpg", GL_NORMAL_MAP, GL_TEXTURE2 );
	objs[4].texture( "res/textures/cubeMap.jpg", GL_TEXTURE_CUBE_MAP, GL_TEXTURE1 );
	objs[5].texture( "res/textures/wood.jpg", GL_TEXTURE_2D, GL_TEXTURE0 );
	objs[1].texture( "res/textures/brick2.jpg", GL_TEXTURE_2D, GL_TEXTURE0 );

	/****************************************************************************
	 * Setup Lighting
	 ***************************************************************************/
	g_lights->addPointLight( vec3( 2.0f, 3.0f, 1.0f )
	                       , vec3( 0.7f, 0.7f, 0.7f )
	                       , 1.0f, 0.0f, 0.0f, 0.1f );
	g_spotlight = g_lights->addSpotLight( g_spotlight_pos
	                                    , vec3( 1.0f, 1.0f, 1.0f )
	                                    ,	1.0f, 0.0f, 0.0f, 0.1f
	                                    , vec3( 0.0f, -1.0f, 0.0f ), 45.0f );
	g_lights->addDirectionalLight( vec3( 0.0f, -1.0f, 0.0f )
	                             , vec3( 0.9f,  0.9f, 0.9f ) );
//  lights->addSpotLight( vec3( 0.0f, 10.0f, 0.0f ), vec3( 1.0f, 1.0f, 1.0f )
//                      , 1.0f, 0.0f, 0.0f, 0.05f, vec3( 0.0f, -1.0f, 0.0f ), 10.0f );
	g_lights->getLights( g_light_array, g_num_of_lights );

	// Camera to get model view and projection matices from. Amongst other things
	g_cam = new Camera( vec3( 0.0f, 2.0f, 25.0f ), g_width, g_height );
	g_cam->setLookCenter();
	/****************************************************************************
	 * Setup Materials
	 ***************************************************************************/
	//torus
	objs[3].materialAmb( 0.5f, 0.0f, 0.0f );
	objs[3].materialReflect( 0.0f );
	objs[3].materialSpec( 0.7f, 0.6f, 0.6f );
	objs[3].materialShininess( 0.25f );
	objs[3].textureBlend( 1.0f );
	//bunny
	objs[1].materialAmb( 0.19225f, 0.19225f, 0.19225f );
	objs[1].materialReflect( 0.5f );
	objs[1].materialSpec( 0.508273f, 0.508273f, 0.508273f );
	objs[1].materialShininess( 0.2f );
	objs[1].textureBlend( 0.5f );
	//sphere
	objs[0].materialAmb( 0.2125f, 0.1275f, 0.054f );
	objs[0].materialReflect( 0.0f );
	objs[0].materialSpec( 0.393548f, 0.271906f, 0.166721f );
	objs[0].materialShininess( 0.2f );
	objs[0].textureBlend( 1.0f );
	//teapot
	objs[4].materialAmb( 0.105882f, 0.058824f, 0.113725f );
	objs[4].materialReflect( 0.5f );
	objs[4].materialSpec( 0.333333f, 0.333333f, 0.521569f );
	objs[4].materialShininess( 0.84615f );
	objs[4].textureBlend( 0.8f );
	//box
	objs[0].materialAmb( 0.05f, 0.05f, 0.05f );
	objs[0].materialReflect( 0.5f );
	objs[0].materialSpec( 1.0f, 1.0f, 1.0f );
	objs[0].materialShininess( 4.0f );
	objs[0].textureBlend( 1.0f );
	//table
	objs[5].materialAmb( 0.05f, 0.05f, 0.05f );
	objs[5].materialReflect( 0.5f );
	objs[5].materialSpec( 1.0f, 1.0f, 1.0f );
	objs[5].materialShininess( 1.0f );
	objs[5].textureBlend( 0.0f );

	///////////////////////////////////////////////////////////////////////////
	//                           Main Rendering Loop                         //
	///////////////////////////////////////////////////////////////////////////
	float black[] =	{ 0, 0, 0 };
	glClearBufferfv( GL_COLOR, 0, black );
	glViewport( 0, 0, g_width, g_height );

	while ( !glfwWindowShouldClose( window ) )
	{
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		// load values into the uniform slots of the shader and draw
		Texture* txt = Texture::getInstance();
		txt->useEnvironmentMap( env, vec3( 5.0f, 1.5f, -5.0f ), objs[2].getReflectID() );
			glUniform1i( env( "image" ), 0 );
			checkGLErrors( 424 );
			glUniform1i( env( "numLights" ), g_num_of_lights );
			checkGLErrors( 426 );
			glUniformMatrix4fv( env( "allLights[0]" ), g_num_of_lights
					, GL_FALSE, g_light_array );
			checkGLErrors( 429 );
			objs[0].drawObject( env );
			objs[1].drawObject( env );
			objs[3].drawObject( env );
			objs[4].drawObject( env );
			objs[5].drawObject( env );
		txt->unUseEnvironmentMap( env, g_width, g_height );

//		shader.use();
//			glUniform1i( shader("image"), 0 );
//			glUniform1i( shader("eMap"), 1 );
//			glUniform1i( shader("normalmap"), 2 );
//			glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE,
//					value_ptr( g_cam->getViewMatrix() ) );
//			glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,
//					value_ptr( g_cam->getProjectionMatrix() ) );
//			glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,
//					value_ptr( g_cam->getNormalMatrix() ) );
//			glUniform1i( shader( "numLights" ), g_num_of_lights );
//			glUniformMatrix4fv( shader( "allLights[0]" ), g_num_of_lights
//					, GL_FALSE, g_light_array );
//			glUniform4fv( shader( "matAmb" ), 1, &bronze[0] );
//			glUniform4fv( shader( "matSpec" ), 1, &bronze[4] );
//			glUniform1f( shader( "matCubemap" ), bronzeCube );
//			glUniform1f( shader( "matNormal" ), bronzeNormal );
//			geo->draw( sphere, 1 );
//
//			// Bunny
//			glUniform4fv( shader( "matAmb" ), 1, &china[0] );
//			glUniform4fv( shader( "matSpec" ), 1, &china[4] );
//			glUniform1f( shader( "matCubemap" ), chinaCube );
//			glUniform1f( shader( "matNormal" ), defNormal );
//			geo->draw( bunny, 1 );
//
//			// Torus
//			glUniform4fv( shader( "matAmb" ), 1, &redplast[0] );
//			glUniform4fv( shader( "matSpec" ), 1, &redplast[4] );
//			glUniform1f( shader( "matCubemap" ), redplasticCube );
//			glUniform1f( shader( "matNormal" ), redplasticNormal );
//			geo->draw( torus, 1 );
//
//			// Teapot
//			glUniform4fv( shader( "matAmb" ), 1, &bMetal[0] );
//			glUniform4fv( shader( "matSpec" ), 1, &bMetal[4] );
//			glUniform1f( shader( "matCubemap" ), metalCube );
//			glUniform1f( shader( "matNormal" ), defNormal );
//			geo->draw( teapot, 1 );
//
//			// Box
//			glUniform4fv( shader( "matAmb" ), 1, &def[0] );
//			glUniform4fv( shader( "matSpec" ), 1, &def[4] );
//			glUniform1f( shader( "matCubemap" ), defCube );
//			glUniform1f( shader( "matNormal" ), defNormal );
//			geo->draw( box, 1 );
//
//			// Table
//			glUniform4fv( shader( "matCubemap" ), 1, &def[0] );
//			glUniform4fv( shader( "matSpec" ), 1, &def[4] );
//			glUniform1f( shader( "matCubemap" ), defCube );
//			glUniform1f( shader( "matNormal" ), defNormal );
//			geo->draw( table, 1 );
//		shader.unUse();

//		buff.BindForWriting();
//		glBindFramebuffer(GL_FRAMEBUFFER, depthBuffer);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		shader.use();
//			glUniform1i( shader("image"), 0 );
//			glUniform1i( shader("eMap"), 1 );
//			glUniform1i( shader("normalmap"), 2 );
//			glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE, value_ptr( g_cam->getViewMatrix() ) );
//			glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,	value_ptr( g_cam->getProjectionMatrix() ) );
//			glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,	value_ptr( g_cam->getNormalMatrix() ) );
//			glUniform1i( shader( "numLights" ), g_num_of_lights );
//			glUniformMatrix4fv( shader( "allLights[0]" ), g_num_of_lights, GL_FALSE, g_light_array );
//			for ( GeoObject& g: objs )
//				g.drawObject( shader );
//		shader.unUse();
//		buff.BindTextures();

		shader.use();
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUniform1i( shader("image"), 0 );
			glUniform1i( shader("eMap"), 1 );
			glUniform1i( shader("normalmap"), 2 );
			// glUniform1i( shader("DepthTexture"), 4 );
			glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE, value_ptr( g_cam->getViewMatrix() ) );
			glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,	value_ptr( g_cam->getProjectionMatrix() ) );
			glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,	value_ptr( g_cam->getNormalMatrix() ) );
			glUniform1i( shader( "numLights" ), g_num_of_lights );
			glUniformMatrix4fv( shader( "allLights[0]" ), g_num_of_lights, GL_FALSE, g_light_array );
			for ( GeoObject& g: objs )
				g.drawObject( shader );
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
