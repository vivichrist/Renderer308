#include "ShaderStorage.hpp"

namespace R308
{
// Size is the number of slots of 4 GLfloats to be buffered
ShaderStorage::ShaderStorage( GLuint binding, uint size )
{
	bindingPoint = binding;
	// create new buffer for Uniform Buffer Object
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	// bind buffer to binding point
	GLsizei s = sizeof(GLfloat) * 4u * size;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizei) s, nullptr, GL_DYNAMIC_DRAW);
}

ShaderStorage::~ShaderStorage()
{
}
template <typename T>
void ShaderStorage::setStorageDataArray( const uint& offset, const T* data, const size_t& size ) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	uint obytes = offset * 4u * sizeof(T);
	uint sbytes = size * sizeof(T);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, obytes, sbytes, data);
}

// both offset and size are number of slots of 4 GLfloats
template <typename T>
void ShaderStorage::setStorageData4( const uint& offset, const T* data, const size_t& size ) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	uint obytes = offset * 4u * sizeof(T);
	uint sbytes = size * 4u * sizeof(T);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, obytes, sbytes, data);
}
// both offset and size are number of slots of 2 GLfloats
template <typename T>
void ShaderStorage::setStorageData2( const uint& offset, const T* data, const size_t& size ) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	uint obytes = offset * 2u * sizeof(T);
	uint sbytes = size * 2u * sizeof(T);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, obytes, sbytes, data);
}
// both offset and size are number of slots of one GLfloat or scalar members
template <typename T>
void ShaderStorage::setStorageData( const uint& offset, const T* data ) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	uint obytes = offset * sizeof(T);
	uint sbytes = sizeof(T);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, obytes, sbytes, data);
}

void ShaderStorage::bindShaderStorage( const GLuint& shaderProgram, const std::string& block )
{
	// index of binding point. Other shaders with same uniform block should use this binding point
	// get index of uniform buffer from shader
	GLuint blockIndex = glGetProgramResourceIndex( shaderProgram, GL_SHADER_STORAGE_BLOCK, block.c_str() );
	// bind uniform block to binding point
	glShaderStorageBlockBinding(shaderProgram, blockIndex, bindingPoint);
}

}