#include "ShaderManager.hpp"

namespace R308
{

ShaderManager* ShaderManager::instance = nullptr;

ShaderManager* ShaderManager::getInstance()
{
	if (instance == nullptr)
		instance = new ShaderManager();
	return instance;
}

ShaderManager::~ShaderManager()
{
    programs.clear();
    uniforms.clear();
    geoMap.clear();
    instance = nullptr;
}

uint ShaderManager::addShader( std::string const& filename, uint const& geoID )
{
	Shader program;
	program.loadFromFile( GL_VERTEX_SHADER, "res/shaders/" + filename + ".vert" );
	program.loadFromFile( GL_TESS_CONTROL_SHADER, "res/shaders/" + filename + ".tcsh" );
	program.loadFromFile( GL_TESS_EVALUATION_SHADER, "res/shaders/" + filename + ".tesh" );
	program.loadFromFile( GL_GEOMETRY_SHADER, "res/shaders/" + filename + ".geom" );
	program.loadFromFile( GL_FRAGMENT_SHADER, "res/shaders/" + filename + ".frag" );
	uint programID = program.createAndLinkProgram();

	programs[programID] = program;
	return programID;
}

void ShaderManager::addUniform(uint const& progID, std::string const& name, uint const& size, uint const& length)
{
	Shader &program = programs[progID];
	program.use();
		program.addUniform( name.c_str(), size, length );
	program.unUse();
	// print debuging info
	program.printActiveUniforms();
}

void ShaderManager::registerGeometry(uint const& geoID, uint const& shID)
{
    Geometry *geo = Geometry::getInstance();
}

} // end R308 namespace