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
vec2 pixSize;
mat4 g_spotlight_rot;
float g_light_array[160];
GLint g_num_of_lights;

// Ambient Occlusion
int aoMode = 0;
const int hemisphereRadius = 4;
const int hemisphereSize = 128;
GLfloat noiseScale[2], g_dof = 0.5;
GLfloat noise[3*hemisphereRadius*hemisphereRadius];
GLuint noiseTex;

// Frame Buffers
uint fbo, stage1fbo, ppfbo[2];
Texture* txt;

int shape = 0;

int g_width = 1024, g_height = 768;

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

	if (key == GLFW_KEY_KP_ADD && (action == GLFW_PRESS || action == GLFW_REPEAT) ){
		g_dof += 0.01f;
		g_dof = g_dof > 1.0f ? 1.0f : g_dof;
		cout << "DOF increase: " << g_dof << "\n";
	}

	if (key == GLFW_KEY_KP_SUBTRACT && (action == GLFW_PRESS || action == GLFW_REPEAT) ){
		g_dof -= 0.01f;
		g_dof = g_dof < 0.0f ? 0.0f : g_dof;
		cout << "DOF decrease: " << g_dof << "\n";
	}
	if (key == GLFW_KEY_O && action == GLFW_PRESS ){
		aoMode++;
		aoMode %= 3;
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS ){
		shape++;
		shape %= 3;
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
	pixSize = vec2( 1.0f/g_width, 1.0f/g_height );
	glViewport( 0, 0, g_width, g_height );
	fbo = txt->setupFBO( g_width, g_height );
	stage1fbo = txt->setupStage1FBO( g_width, g_height );
	txt->setupPinPongFBO( g_width, g_height, ppfbo[0], ppfbo[1] );
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

float randPoint() {
    return ((float) rand()) / (float) RAND_MAX;
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
	glfwSwapInterval( 2 );

	glfwSetErrorCallback( error_callback );
	glfwSetKeyCallback( window, key_callback );
	glfwSetScrollCallback( window, scroll_callback );
	glfwSetMouseButtonCallback( window, mousebutton_callback );
	glfwSetCursorPosCallback( window, mousemotion_callback );
	glfwSetFramebufferSizeCallback( window, resize_callback );
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glDepthFunc( GL_LEQUAL );

	srand(time(NULL));

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
		shader.addUniform( "image" );
	shader.unUse();
	// print debugging info
	shader.printActiveUniforms();

	Shader postShader;
	postShader.loadFromFile( GL_VERTEX_SHADER, "vertex_pdef.glsl" );
	postShader.loadFromFile( GL_GEOMETRY_SHADER, "geometry_pdef.glsl" );
	postShader.loadFromFile( GL_FRAGMENT_SHADER, "fragment_pdef.glsl" );
	postShader.createAndLinkProgram();
	postShader.use();
		postShader.addUniform( "depth" );
		postShader.addUniform( "colour" );
		postShader.addUniform( "normal" );
		postShader.addUniform( "pixelSize" );
		postShader.addUniform( "aoMode" );
		postShader.addUniform( "projMat" );
		postShader.addUniform( "noiseScale" );
		postShader.addUniform( "noiseTexture" );
		postShader.addUniform( "hemisphere" );
		postShader.addUniform( "hemisphereSize" );
		postShader.addUniform( "hemisphereRadius" );
	postShader.unUse();
	// print debugging info
	shader.printActiveUniforms();

	Shader ppShader;
	ppShader.loadFromFile( GL_VERTEX_SHADER, "vertex_pdef.glsl" );
	ppShader.loadFromFile( GL_GEOMETRY_SHADER, "geometry_pdef.glsl" );
	ppShader.loadFromFile( GL_FRAGMENT_SHADER, "fragment_pp.glsl" );
	ppShader.createAndLinkProgram();
	ppShader.use();
		ppShader.addUniform( "colour" );
		ppShader.addUniform( "spec" );
		ppShader.addUniform( "pixelSize" );
		ppShader.addUniform( "isVert" );
	ppShader.unUse();
	// print debugging info
	ppShader.printActiveUniforms();

	Shader combine;
	combine.loadFromFile( GL_VERTEX_SHADER, "vertex_pdef.glsl" );
	combine.loadFromFile( GL_GEOMETRY_SHADER, "geometry_pdef.glsl" );
	combine.loadFromFile( GL_FRAGMENT_SHADER, "fragment_combine.glsl" );
	combine.createAndLinkProgram();
	combine.use();
    combine.addUniform( "depth" );
    combine.addUniform( "colour" );
    combine.addUniform( "blurColour" );
    combine.addUniform( "blurSpec" );
    combine.addUniform( "dof" );
	combine.unUse();
	// print debugging info
	combine.printActiveUniforms();
	/****************************************************************************
	 * Setup Geometry
	 ***************************************************************************/
	//g_spotlight_pos = vec3( 0.0f, 7.0f, 0.0f );
	Geometry *geo = Geometry::getInstance();
	uint bunny = geo->addBuffer( "res/assets/bunny.obj"
			, vec3( 0.0f, -1.5f, 0.0f )
			, vec3( 0.80754f, 0.90754f, 0.90754f ) );
	uint teapot = geo->addBuffer( "res/assets/teapot.obj"
			, vec3( 0.0f, -1.5f, 0.0f )
			, vec3( 0.80754f, 0.90754f, 0.90754f ) );
	uint torus = geo->addBuffer( "res/assets/torus.obj"
			, vec3( 0.0f, -1.0f, 0.0f )
			, vec3( 0.80754f, 0.90754f, 0.90754f ) );
	uint sphere = geo->addBuffer( "res/assets/sphere.obj"
            , vec3( 0.0f, 2.0f, 0.0f ) );
	uint table = geo->addBuffer( "res/assets/table.obj"
            , vec3( 0.0f, -2.0f, 0.0f ) );

	if ( checkGLErrors( 375 ) ) exit(1);

	txt = Texture::getInstance();

	geo->bindTexure( "res/textures/brick.jpg", sphere );
	geo->bindTexure( "res/textures/wood.jpg", table );

	/****************************************************************************
	 * Setup Lighting
	 ***************************************************************************/
	g_lights->addPointLight( vec3( 5.0f, 5.0f, -5.0f )
	                       , vec3( 1.5f, 1.5f, 1.5f )
	                       , 2.0f, 0.0f, 0.0f, 0.1f );
	g_lights->addDirectionalLight( vec3( 0.0f, -1.0f, 0.0f )
	                             , vec3( 1.5f,  1.5f, 1.5f ) );
	g_lights->getLights( g_light_array, g_num_of_lights );

	// Camera to get model view and projection matices from. Amongst other things
	g_cam = new Camera( vec3( 0.0f, 2.0f, 10.0f ), g_width, g_height );
	g_cam->setupProjection( M_PI_4, (float) g_width / (float) g_height
						, 0.5f, 100.0f );
	g_cam->setLookCenter();

	///////////////////////////////////////////////////////////////////////////
	//                           Main Rendering Loop                         //
	///////////////////////////////////////////////////////////////////////////
	float black[] =	{ 0, 0, 0 };
	vec3 lightPos( 5, 2, 5 );
	float blur[] = {
		  1, 4, 7, 4, 1
		, 4, 16, 26, 16, 4
		, 7, 26, 41, 26, 7
		, 4, 16, 26, 16, 4
		, 1, 4, 7, 4, 1
	};
	for ( float& f: blur ) f /= 273.0f;
	glClearBufferfv( GL_COLOR, 0, black );
	glViewport( 0, 0, g_width, g_height );
	fbo = txt->setupFBO( g_width, g_height );
	stage1fbo = txt->setupStage1FBO( g_width, g_height );
	txt->setupPinPongFBO( g_width, g_height, ppfbo[0], ppfbo[1] );

    GLfloat samplePoints[3 * hemisphereSize];
	for(int i = 0; i < hemisphereSize; i++) {
		vec3 random = normalize(vec3( randPoint(), randPoint(), randPoint()));

		float scale = float(i/3) / (float)hemisphereSize;
		scale = 0.1 + scale*scale*0.9;

		random *= scale;

		samplePoints[i*3] = random.x;
		samplePoints[i*3 + 1] = random.y;
		samplePoints[i*3 + 2] = random.z;
	}

	for(int i = 0; i < hemisphereRadius*hemisphereRadius; i++) {
		vec3 random = normalize(vec3(randPoint(), randPoint(), 0.0));

		noise[i*3] = random.x;
		noise[i*3 + 1] = random.y;
		noise[i*3 + 2] = random.z;
	}

	pixSize.x = 1.0f/g_width;
	pixSize.y = 1.0f/g_height;

	noiseScale[0] = g_width / 4.0;
	noiseScale[1] = g_height / 4.0;

	glActiveTexture(GL_TEXTURE8);
	glGenTextures(1, &noiseTex);
	glBindTexture(GL_TEXTURE_2D, noiseTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, hemisphereRadius, hemisphereRadius, 0, GL_RGB, GL_FLOAT, noise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE0);

	float lightRot = M_1_PI / 30;
	int i;
	while ( !glfwWindowShouldClose( window ) )
	{
		// load values into the uniform slots of the shader and draw
		txt->activateFrameBuffer( fbo );

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable( GL_DEPTH_TEST );
		shader.use();
			glUniform1i( shader("image"), 0 );
			glUniformMatrix4fv( shader( "mvM" ), 1, GL_FALSE, value_ptr( g_cam->getViewMatrix() ) );
			glUniformMatrix4fv( shader( "projM" ), 1, GL_FALSE,	value_ptr( g_cam->getProjectionMatrix() ) );
			glUniformMatrix3fv( shader( "normM" ), 1, GL_FALSE,	value_ptr( g_cam->getNormalMatrix() ) );
			glUniform3f( shader( "lightP" ), lightPos.x, lightPos.y, lightPos.z );

			if (shape == 0) geo->draw( bunny, 1 );
			else if (shape == 1) geo->draw( sphere, 1 );
			else if (shape == 2) geo->draw( torus, 1 );
			else geo->draw( teapot, 1 );

			geo->draw( table, 1 );
		shader.unUse();
		txt->activateTexturesFB( fbo );
		lightPos = rotateY( lightPos, lightRot );

		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, noiseTex);

		txt->activateFrameBuffer( stage1fbo );
		glActiveTexture(GL_TEXTURE0);
		glClear( GL_DEPTH_BUFFER_BIT );
		glDisable( GL_DEPTH_TEST );

		postShader.use();
			glUniform1i( postShader("depth"), 0 );
			glUniform1i( postShader("colour"), 1 );
			glUniform1i( postShader("normal"), 2 );
			glUniform1i( postShader("noiseTexture"), 8 );
			glUniform1i( postShader("aoMode"), aoMode );
			glUniform3fv( postShader("hemisphere"), hemisphereSize, samplePoints);
			glUniformMatrix4fv( postShader( "projMat" ), 1, GL_FALSE, value_ptr( g_cam->getProjectionMatrix() ) );
			glUniform2f( postShader("noiseScale"), noiseScale[0], noiseScale[1] );
			glUniform1i( postShader("hemisphereSize"), hemisphereSize );
			glUniform1i( postShader("hemisphereRadius"), hemisphereRadius );
			glDrawArrays(GL_POINTS, 0, 1);
		postShader.unUse();

//    glClear( GL_DEPTH_BUFFER_BIT );
    ppShader.use();
      glUniform1i( ppShader("colour"), 0 );
      glUniform1i( ppShader("spec"), 1 );
      glUniform2f( ppShader("pixelSize"), pixSize.x, pixSize.y);

      glUniform1i( ppShader("isVert"), 0 );
      txt->activateStage2FB( stage1fbo, fbo, ppfbo[0] );
      glDisable( GL_DEPTH_TEST );
      //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      glDrawArrays(GL_POINTS, 0, 1);

      for ( i = 0; i<9; ++i )
      {
        glUniform1i( ppShader("isVert"), i % 2 );
        txt->swapPPFBO( ppfbo[(i + 1) % 2], ppfbo[i % 2] );
        //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        glDrawArrays(GL_POINTS, 0, 1);
	    }
    ppShader.unUse();

    // setup combine shader with the last shader as input
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    txt->combineStage( fbo, stage1fbo, ppfbo[0] );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDisable( GL_DEPTH_TEST );
    combine.use();
      glUniform1i( combine("depth"), 0 );
      glUniform1i( combine("colour"), 1 );
      glUniform1i( combine("blurColour"), 2 );
      glUniform1i( combine("blurSpec"), 3 );
      glUniform1f( combine("dof"), g_dof );
      glDrawArrays(GL_POINTS, 0, 1);
    combine.unUse();

		txt->deactivateTexturesFB();

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
