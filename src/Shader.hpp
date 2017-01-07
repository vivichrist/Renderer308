/*
 * Shader.h
 *
 *  Created on: 16/01/2015
 *      Author: vivichrist
 */

#pragma once

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
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

	void loadFromString( GLenum const& whichShader, std::string const& source );
	void loadFromFile( GLenum const& whichShader, std::string const& filename );
	uint createAndLinkProgram();
	void setupShader( const char**, const GLenum[]
					, uint const&, char const**, uint const& );
	void use();
	void unUse();
	void addAttribute( std::string const& attribute );
	void addUniform( std::string const& uniform, uint const& layout, size_t const& length );
	void setUniform(std::string const &uniform, float const *data);
	void registerFragOut( uint const&, std::string const& );
	GLint operator[]( std::string const& attribute );
	GLint operator()( std::string const& uniform );
	void printActiveUniforms();
private:
	bool getShader( std::string const& filename, std::string& source );
	void checkShader( GLuint const& shaderName, std::string const& name );
	void checkProgram();

	GLuint pgName;
	GLuint totalShaders;
	bool completed;
	GLuint shaders[5];
	std::map<std::string, GLint> attributeList;
	std::map<std::string, GLint> uniformLocationList;
	std::map<std::string, std::pair<uint, size_t>> uniformSizeList;
};
