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
int kernelSize = 32;

GLfloat parallaxScale = 0.05;
GLfloat parallaxMinLayer = 20;
GLfloat parallaxMaxLayer = 50;
bool aoRight = false;

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
	else if ( key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS )
	{
	  parallaxScale -= 0.05;
	}
	//
	// Parallax Commands
	//
	else if ( key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS )
	{
		parallaxScale += 0.05;
		cerr << "Parallax Scale: " << parallaxScale << endl;
	}
	else if ( key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS )
	{
		parallaxScale -= 0.05;
		cerr << "Parallax Scale: " << parallaxScale << endl;
	}
	else if ( key == GLFW_KEY_HOME && action == GLFW_PRESS )
	{
		parallaxMaxLayer += 5;
		cerr << "Parallax Max Layer: " << parallaxMaxLayer << endl;
	}
	else if ( key == GLFW_KEY_END && action == GLFW_PRESS )
	{
		float p = parallaxMaxLayer - 5;
		parallaxMaxLayer = p > parallaxMinLayer ? p : parallaxMaxLayer;
		cerr << "Parallax Max Layer: " << parallaxMaxLayer << endl;
	}
	else if ( key == GLFW_KEY_INSERT && action == GLFW_PRESS )
	{
		float p = parallaxMinLayer + 5;
		parallaxMinLayer = p < parallaxMinLayer ? p : parallaxMinLayer;
		cerr << "Parallax Min Layer: " << parallaxMinLayer << endl;
	}
	else if ( key == GLFW_KEY_DELETE && action == GLFW_PRESS )
	{
		float p = parallaxMinLayer - 5;
		parallaxMinLayer = p < 0 ? 0 : p;
		cerr << "Parallax Min Layer: " << parallaxMaxLayer << endl;
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

	if (key == GLFW_KEY_P && action == GLFW_PRESS )aoRight = !aoRight;
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
	shader.loadFromFile( GL_VERTEX_SHADER, "vertex.glsl" );
	shader.loadFromFile( GL_FRAGMENT_SHADER, "fragment_def.glsl" );
	shader.createAndLinkProgram();
	shader.use();
		shader.addUniform( "mvM" );
		shader.addUniform( "projM" );
		shader.addUniform( "normM" );
		shader.addUniform( "lightP" );
		shader.addUniform( "viewP" );
		shader.addUniform( "image" );
		shader.addUniform( "normalmap" );
		shader.addUniform( "heightmap" );
		shader.addUniform( "parallaxScale" );
		shader.addUniform( "parallaxMinLayer" );
		shader.addUniform( "parallaxMaxLayer" );
	shader.unUse();
	// print debugging info
	shader.printActiveUniforms();

	Shader shader2;
	shader2.loadFromFile( GL_VERTEX_SHADER, "vertex_pdef.glsl" );
	shader2.loadFromFile( GL_GEOMETRY_SHADER, "geometry_pdef.glsl" );
	shader2.loadFromFile( GL_FRAGMENT_SHADER, "fragment_pdef.glsl" );
	shader2.createAndLinkProgram();
	shader2.use();
		shader2.addUniform( "depth" );
		shader2.addUniform( "colour" );
		shader2.addUniform( "normal" );
		shader2.addUniform( "eye" );
		shader2.addUniform( "pixelSize" );
		shader2.addUniform( "aoRight" );
	shader2.unUse();
	// print debugging info
	shader.printActiveUniforms();
	/****************************************************************************
	 * Setup Geometry
	 ***************************************************************************/
	//g_spotlight_pos = vec3( 0.0f, 7.0f, 0.0f );
	Geometry *geo = Geometry::getInstance();
	uint table = geo->addBuffer( "res/assets/box.obj"
	                            , vec3( 0.0f, -0.5f, 0.0f ) );

	if ( checkGLErrors( 375 ) ) exit(1);

	Texture *txt = Texture::getInstance();


	geo->bindTexure( "res/textures/brick2.jpg", table );
	geo->bindNMTexure( "res/textures/brick2_normal.jpg", table );
	geo->bindHMTexure( "res/textures/brick2_height.jpg", table );

	/****************************************************************************
	 * Setup Lighting
	 ***************************************************************************/
//	g_lights->addPointLight( vec3( 5.0f, 5.0f, -5.0f )
//	                       , vec3( 1.5f, 1.5f, 1.5f )
//	                       , 2.0f, 0.0f, 0.0f, 0.1f );
//	g_spotlight = g_lights->addSpotLight( g_spotlight_pos
//	                                    , vec3( 1.0f, 1.0f, 1.0f )
//	                                    ,	1.0f, 0.0f, 0.0f, 0.1f
//	                                    , vec3( 0.0f, -1.0f, 0.0f ), 45.0f );
//	g_lights->addDirectionalLight( vec3( 0.0f, -1.0f, 0.0f )
//	                             , vec3( 1.5f,  1.5f, 1.5f ) );
//  lights->addSpotLight( vec3( 0.0f, 10.0f, 0.0f ), vec3( 1.0f, 1.0f, 1.0f )
//                      , 1.0f, 0.0f, 0.0f, 0.05f, vec3( 0.0f, -1.0f, 0.0f ), 10.0f );
//	g_lights->getLights( g_light_array, g_num_of_lights );

	// Camera to get model view and projection matices from. Amongst other things
	g_cam = new Camera( vec3( 0.0f, 2.0f, 10.0f ), g_width, g_height );
	g_cam->setLookCenter();
//	float redplast[] = { 0.0f, 0.0f, 0.0f, 0.0f // ambient + reflect
//	                   , 0.7f, 0.6f, 0.6f, 0.25f // specular + shininess
//	                   , 0.0f, 0.0f, 0.0f, 0.0f}; // cubemap
//	GLfloat redplasticCube = 0.0f;
//	GLfloat redplasticNormal = 1.0f;
//
//	GLfloat bronzeCube = 0.0f;
//	GLfloat bronzeNormal = 1.0f;
//
//	GLfloat chinaCube = 0.5f;
//	GLfloat metalCube = 0.8f;
//
//	float bronze[] = { 0.2125f, 0.1275f, 0.054f, 0.0f
//	                , 0.393548f, 0.271906f, 0.166721f, 0.2f };
//	float china[] = { 0.19225f, 0.19225f, 0.19225f, 0.5f
//	                , 0.508273f, 0.508273f, 0.508273f, 0.2f };
//	float bMetal[] = { 0.105882f, 0.058824f, 0.113725f, 0.5f
//	                , 0.333333f, 0.333333f, 0.521569f, 0.84615f };
//	float def[] 	= { 0.05f, 0.05f, 0.05f, 0.0f
//		                , 1.0f, 1.0f, 1.0f, 4.0f
//		                , 0.0f, 0.0f, 0.0f, 0.0f}; // cubemap };
//	GLfloat defCube = 0.5f;
//	GLfloat defNormal = 0.0f;

	///////////////////////////////////////////////////////////////////////////
	//                           Main Rendering Loop                         //
	///////////////////////////////////////////////////////////////////////////

//	GLuint vao, vbo;
//	glGenVertexArrays( 1, &vao );
//	glBindVertexArray(vao);
//	// initialise/bind vertex buffer object
//	glGenBuffers(1, vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	// write data
//	float quad[] = {  -1.0f, -1.0f, 0.0f
//					, 1.0f, -1.0f, 0.0f
//					, 1.0f, 1.0f, 0.0f
//					,-1.0f, 1.0f, 0.0f };
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, quad, GL_STATIC_DRAW);
//	glBindVertexArray(0);

	float black[] =	{ 0, 0, 0 };
	float lightPos[] =	{ 0, 10, 0 };
	glClearBufferfv( GL_COLOR, 0, black );
	glViewport( 0, 0, g_width, g_height );
	uint fbo = txt->setupFBO( g_width, g_height );
	GLfloat ssaoKernel[3 * kernelSize];

	srand(time(NULL));
	for(int i = 0; i < kernelSize; i++) {
		vec3 randVec = vec3( rand() * 2.0 - 1.0,
				rand() * 2.0 - 1.0,
				rand());

		normalize(randVec);

		float scale = float(i/3) / (float)kernelSize;
		scale = 0.1 + scale*scale*0.9;

		randVec *= scale;

		ssaoKernel[i*3] = randVec.x;
		ssaoKernel[i*3 + 1] = randVec.y;
		ssaoKernel[i*3 + 2] = randVec.z;
	}

	while ( !glfwWindowShouldClose( window ) )
	{
		// load values into the uniform slots of the shader and draw
		txt->activateFrameBuffer( fbo );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		shader.use();
			glUniform1i( shader("image"), 0 );
			glUniform1i( shader("normalmap"), 1 );
			glUniform1i( shader("heightmap"), 2 );
			glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE,
					value_ptr( g_cam->getViewMatrix() ) );
			glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,
					value_ptr( g_cam->getProjectionMatrix() ) );
			glUniformMatrix4fv( shader( "viewP" ), 1, GL_FALSE,
					value_ptr( g_cam->getPosition() ) );
			glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,
					value_ptr( g_cam->getNormalMatrix() ) );
			glUniform3fv( shader( "lightP" ), 1, lightPos );
			glUniform1f( shader( "parallaxScale" ), parallaxScale );
			glUniform1f( shader( "parallaxMinLayer" ), parallaxMinLayer );
			glUniform1f( shader( "parallaxMaxLayer" ), parallaxMaxLayer );
			geo->draw( table, 1 );
		shader.unUse();
		txt->activateTexturesFB( fbo );

		glClear( GL_DEPTH_BUFFER_BIT );

		shader2.use();
			glUniform1i( shader2("depth"), 0 );
			glUniform1i( shader2("colour"), 1 );
			glUniform1i( shader2("normal"), 2 );
			glUniform1i( shader2("eye"), 3 );
			glUniform1i( shader2("aoRight"), aoRight );
			glUniform3fv( shader2("kernel"), kernelSize, ssaoKernel);
			glUniform2f( shader2("pixelSize"), 1.0/g_width, 1.0/g_height);
			glDrawArrays(GL_POINTS, 0, 1);
		shader2.unUse();

		//txt->deactivateTexturesFB();

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
