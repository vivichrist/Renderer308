/*
 * Shader.cpp
 *
 *  Created on: 16/01/2015
 *      Author: vivichrist
 */

#include "Shader.hpp"

Shader::Shader() : totalShaders(0), completed(false)
{
	pgName = glCreateProgram();
}

Shader::~Shader()
{
    deleteShaderProgram();
}
///////////////////////////////////////////////////////////////////////////////
//                          Private Helper Methods                           //
///////////////////////////////////////////////////////////////////////////////
void Shader::getShader( const std::string& filename, std::string& source )
{
	std::ifstream pg_in( filename.c_str() );
	if ( !pg_in.is_open() or pg_in.bad() )
	{
		std::stringstream str;
		str << "File " << filename.c_str() << " Not Found";
		throw str.str().c_str();
	}
	std::string line;
	while ( std::getline( pg_in, line ) )
	{
		source.append( line + '\n' );
		//size_t pos = -1;
		//if ((pos = line.find("uniform", 0)) >= 0)
		// TODO: automatically add uniforms...	
	}
	pg_in.close();
}

void Shader::checkShader( const GLuint shaderName, const std::string& name )
{
	GLint isCompiled = 0;
	glGetShaderiv( shaderName, GL_COMPILE_STATUS, &isCompiled );
	if ( isCompiled == GL_FALSE )
	{
		GLint maxLength = 0;
		glGetShaderiv( shaderName, GL_INFO_LOG_LENGTH, &maxLength );

		// The maxLength includes the NULL character
		GLchar errorLog[ maxLength ] = { 0 };
		glGetShaderInfoLog( shaderName, maxLength, &maxLength, &errorLog[0] );

		//print infoLog and end execution.
		std::stringstream str;
		str << "shader object " << name << " failed to compile\n" << errorLog;
		// Exit with failure.
		glDeleteShader( shaderName ); // Don't leak the shader.
		std::cout << str.str().c_str();
		throw;
	}
	++totalShaders;
}

void Shader::setupShader( const char** glslfiles
						, const GLenum shadertype[]
						, const uint numfs
						, const char** uniforms
						, const uint numshd )
{
	uint i = 0;
	for ( ; i<numshd; ++i )
		loadFromFile( shadertype[i], glslfiles[i] );
	createAndLinkProgram();
	use();
	i = 0;
	for ( ; i<numfs; ++i )
		addUniform( uniforms[i] );
	unUse();
	printActiveUniforms();
}

void Shader::checkProgram()
{
	GLint isLinked = 0;
	glGetProgramiv( pgName, GL_LINK_STATUS, (int *) &isLinked );
	if ( isLinked == GL_FALSE )
	{
		GLint maxLength = 0;
		glGetProgramiv( pgName, GL_INFO_LOG_LENGTH, &maxLength );
		//The maxLength includes the NULL character
		GLchar infoLog[ maxLength ] = { 0 };
		glGetProgramInfoLog( pgName, maxLength, &maxLength, &infoLog[0] );
		//remove debris.
		deleteShaderProgram();
		//print infoLog and end execution.
		std::stringstream str;
		str << "\nprogram object " << pgName << " failed to link" << infoLog << "\n";
		std::cout << str.str().c_str();
		throw;
	}
}
/******************************************************************************
 * Tell opengl we wish to use this shader to shade the following draw.
 */
void Shader::use()
{
	if ( !completed )
	{
		throw "Program Not Complete! Cannot Use.";
	}
	glUseProgram( pgName );
}
/******************************************************************************
 * Tell opengl we are finished with this shader.
 */
void Shader::unUse()
{
	glUseProgram( 0 );
}
/******************************************************************************
 * Upload a shader program to the graphics card ready for compilation
 * @param whichShader on of GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
 *        , GL_GEOMETRY_SHADER
 * @param source a string holding the source code for a GLSL shader
 */
void Shader::loadFromString( const GLenum whichShader, const std::string& source )
{
	if ( source.empty() or !(whichShader == GL_VERTEX_SHADER
						  or whichShader == GL_FRAGMENT_SHADER
						  or whichShader == GL_TESS_CONTROL_SHADER
						  or whichShader == GL_TESS_EVALUATION_SHADER
						  or whichShader == GL_COMPUTE_SHADER
						  or whichShader == GL_GEOMETRY_SHADER) )
		throw "file names for shader objects must be valid";
	GLuint name = glCreateShader( whichShader );
	const GLchar * src = source.c_str();
	shaders[totalShaders] = name;
	glShaderSource( name, 1, &src, nullptr );
	glCompileShader( name );
	checkShader( name, source );
}
/******************************************************************************
 *
 * @param whichShader on of GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
 *        , GL_GEOMETRY_SHADER
 * @param filename of a file containing GLSL source code
 */
void Shader::loadFromFile( const GLenum whichShader, const std::string& filename )
{
	if ( filename.empty() or !(whichShader == GL_VERTEX_SHADER
							or whichShader == GL_FRAGMENT_SHADER
							or whichShader == GL_TESS_CONTROL_SHADER
							or whichShader == GL_TESS_EVALUATION_SHADER
							or whichShader == GL_COMPUTE_SHADER
							or whichShader == GL_GEOMETRY_SHADER) )
		throw "file names for shader objects must be valid";
	GLuint name = glCreateShader( whichShader );
	shaders[totalShaders] = name;
	std::string shader;
	getShader( filename, shader );
	const GLchar * shaderSource = shader.c_str();
	glShaderSource( name, 1, &shaderSource, nullptr );
	glCompileShader( name );
	checkShader( name, filename );
}
/******************************************************************************
 * If the default number of shaders are loaded. Compile and link the shader
 * program from the shaders previously loaded.
 */
uint Shader::createAndLinkProgram()
{
	if ( totalShaders < 2 )
	{
		std::stringstream str;
		str << "program object " << pgName << " has too few shaders";
		throw str.str().c_str();
	}
	for ( GLuint i = 0; i < totalShaders; ++i )
		glAttachShader( pgName, shaders[i] );
	glLinkProgram( pgName );
	checkProgram();
	for ( GLuint i = 0; i < totalShaders; ++i )
		glDetachShader( pgName, shaders[i] );
	completed = true;
	return(pgName);
}
/******************************************************************************
 * Register with the graphics card a location where data can be accepted in the
 * GPU pipeline. This is usually unnecessary if within the shader code
 * locations have been marked with identifying number id's.
 * @param attribute
 */
void Shader::addAttribute( const std::string& attribute )
{

	if ( !completed )
	{
		std::stringstream str;
		str << "program " << pgName << " has not been linked yet and '"
			<< attribute << "' cannot be attributed";
		throw str.str().c_str();
	}
	GLint location = glGetAttribLocation( pgName, attribute.c_str() );
	if ( location == -1 )
	{
		std::cout << "Could not add attribute: " << attribute
			<< " - location returned -1!" << std::endl;
	}
	else
		attributeList[ attribute ] = location;
}
/******************************************************************************
 * Register the names of recipient uniform locations in shader code.
 * @param uniform
 */
void Shader::addUniform( const std::string& uniform )
{
	if ( !completed )
	{
		std::stringstream str;
		str << "program " << pgName << " has not been linked yet and '"
			<< uniform << "' cannot be added as a uniform";
		throw str.str().c_str();
	}
	const GLchar * name = uniform.c_str();
	GLint location = glGetUniformLocation( pgName, name );
	if ( location == -1 )
	{
		std::cout << "Could not add uniform: " << uniform
				  << " - location returned -1!" << std::endl;
	}
	else
		uniformLocationList[ uniform ] = location;
}
/******************************************************************************
 * Get the id (name) of a named attribute within the shader. usually
 * unnecessary if these are already known within the code.
 * i.e ...(location = 0)...
 * @param attribute
 * @return id
 */
void Shader::registerFragOut( uint loc, const std::string& called )
{
	glBindFragDataLocation( pgName, loc, called.c_str());
}

GLuint Shader::operator []( const std::string& attribute )
{
	std::map<std::string, GLuint>::iterator it =
			attributeList.find(attribute);
	if ( it != attributeList.end() )
		return( attributeList[attribute] );
	else
	{
		std::cout << "Could not find attribute in shader program: "
				  << attribute << std::endl;
	}
	return( -1 );
}
/******************************************************************************
 * Get the id (name or location) of a named Uniform Constant within the shader.
 * @param uniform
 * @return id
 */
GLuint Shader::operator ()( const std::string& uniform )
{
	std::map<std::string, GLuint>::iterator it =
			uniformLocationList.find(uniform);
	if ( it != uniformLocationList.end() )
		return( uniformLocationList[uniform] );
	else
	{
		std::cout << "Could not find uniform in shader program: " << uniform
				  << std::endl;
	}
	return( -1 );
}
/******************************************************************************
 * Useful debugging information
 */
void Shader::printActiveUniforms()
{
    GLint nUniforms, size, location, maxLen;
    GLsizei written;
    GLenum type;

    glGetProgramiv( pgName, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen );
    glGetProgramiv( pgName, GL_ACTIVE_UNIFORMS, &nUniforms );

    GLchar name[ maxLen ];

    std::cout << " Location | Name\n";
    std::cout << "------------------------------------------------\n";

    for( int i = 0; i < nUniforms; ++i )
    {
		glGetActiveUniform( pgName, i, maxLen, &written, &size, &type,
				name );
		location = glGetUniformLocation( pgName, name );
		std::cout << location << " - " << name << "\n" << std::endl;
    }
}

void Shader::deleteShaderProgram()
{
	glDeleteProgram( pgName );
	glDeleteShader( shaders[0] );
	glDeleteShader( shaders[1] );
	glDeleteShader( shaders[2] );
	glDeleteShader( shaders[3] );
	glDeleteShader( shaders[4] );
}
