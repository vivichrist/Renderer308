#include "ShaderStorage.hpp"

namespace R308
{
// Size is the number of slots of 4 Ts to be buffered
ShaderStorage::ShaderStorage( GLuint binding, uint size )
{
	bindingPoint = binding;
	// create new buffer for Uniform Buffer Object
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	// bind buffer to binding point
	GLsizei s = 16u * size;
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, (GLsizei) s, nullptr, GL_DYNAMIC_DRAW);
}

ShaderStorage::~ShaderStorage()
{
    glDeleteBuffers(1u, &buffer);
}
// both offset and size are number of slots of vec4 so size==4 -> mat4
// size==3 -> mat3 or mat3x4 or mat3x2 (assumes data is padded out)
template<typename T>
void ShaderStorage::setStorageData( T const *data, uint const &col, uint const &row, uint const &offset )
{
	uint obytes, sbytes;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	if (col == 3)
	{
		uint stride = 4u;
		sbytes = stride * sizeof(T);
		for (uint i = 0; i < row; ++i)
		{
			obytes = (offset + i) * stride * sizeof(T);
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, obytes, sbytes, data + (i * row) );
		}
		return;
	}
	sbytes = col * row * sizeof(T);
	obytes = offset * col * sizeof(T);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, obytes, sbytes, data );
}

template<typename T>
void ShaderStorage::setStorageDataArray(T const* data, uint const& offset, size_t const& size)
{
	uint obytes, sbytes;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
	sbytes = sizeof(T);
	obytes = offset * size * sizeof(T);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, obytes, sbytes, data );
}

void ShaderStorage::bindShaderStorage( const GLuint& shaderProgram, const std::string& block )
{
	// index of binding point. Other shaders with same uniform block should use this binding point
	// get index of uniform buffer from shader
	GLuint blockIndex = glGetProgramResourceIndex( shaderProgram, GL_SHADER_STORAGE_BLOCK, block.c_str() );
	// bind uniform block to binding point
	glShaderStorageBlockBinding(shaderProgram, blockIndex, bindingPoint);
}

template void ShaderStorage::setStorageData<GLfloat>(GLfloat const *data, uint const &col, uint const &row, uint const &offset);
template void ShaderStorage::setStorageData<GLuint>(GLuint const *data, uint const &col, uint const &row, uint const &offset);
template void ShaderStorage::setStorageData<GLint>(GLint const *data, uint const &col, uint const &row, uint const &offset);
template void ShaderStorage::setStorageDataArray<GLfloat>(GLfloat const *data, uint const &offset, size_t const &size);
template void ShaderStorage::setStorageDataArray<GLuint>(GLuint const *data, uint const &offset, size_t const &size);
template void ShaderStorage::setStorageDataArray<GLint>(GLint const *data, uint const &offset, size_t const &size);

}