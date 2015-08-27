/*
 * Shader.h
 *
 *  Created on: 16/01/2015
 *      Author: vivichrist
 */

#ifndef SHADER_HPP_
#define SHADER_HPP_

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <exception>
#include <vector>
#include <iostream>   // file I/O
#include <fstream>   // file I/O
#include <sstream>
#include <string>
#include <map>


class Shader
{
public:
	Shader();
	~Shader();

	void loadFromString( const GLenum whichShader, const std::string& source );
	void loadFromFile( const GLenum whichShader, const std::string& filename );
	void createAndLinkProgram();
	void use();
	void unUse();
	void addAttribute( const std::string& attribute );
	void addUniform( const std::string& uniform );
	GLuint operator[]( const std::string& attribute );
	GLuint operator()( const std::string& uniform );
	void printActiveUniforms();
	void deleteShaderProgram();
private:
	void getShader( const std::string& filename, std::string& source );
	void checkShader( const GLuint shaderName, const std::string& name );
	void checkProgram();

	GLuint pgName;
	GLuint totalShaders;
	bool completed;
	GLuint shaders[3];
	std::map<std::string, GLuint> attributeList;
	std::map<std::string, GLuint> uniformLocationList;
};

#endif /* SHADER_HPP_ */
