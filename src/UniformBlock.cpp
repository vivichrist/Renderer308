#include "UniformBlock.hpp"

namespace R308
{
// Size is the number of slots of vec4 to be buffered
UniformBlock::UniformBlock( GLuint binding, uint size )
{
	bindingPoint = binding;
	// create new buffer for Uniform Buffer Object
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	// bind buffer to binding point
	GLsizei s = sizeof(GLfloat) * 4u * size;
	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, buffer);
	glBufferData(GL_UNIFORM_BUFFER, (GLsizei) s, nullptr, GL_DYNAMIC_DRAW);
}

UniformBlock::~UniformBlock()
{
}

// both offset and size are number of slots of vec4 so size==4 -> mat4
// size==3 -> mat3 or mat3x4 or mat3x2 (assumes data is padded out)
template <typename T>
void UniformBlock::setUniformData( const uint &col, const uint &row, const uint &offset, T *data )
{
    uint obytes, sbytes;
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
	if (col == 3)
	{
		uint stride = 4u;
		sbytes = stride * sizeof(T);
		for (uint i = 0; i < row; ++i)
		{
			obytes = (offset + i) * stride * sizeof(T);
			glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data + (i * row) );
		}
		return;
	}
	sbytes = col * row * sizeof(T);
	obytes = offset * col * sizeof(T);
	glBufferSubData(GL_UNIFORM_BUFFER, obytes, sbytes, data );
}

void UniformBlock::bindUniformBlock( const GLuint& shaderProgram, const std::string& block )
{
	// index of binding point. Other shaders with same uniform block should use this binding point
	// get index of uniform buffer from shader
	GLuint blockIndex = glGetUniformBlockIndex(shaderProgram, block.c_str() );
	// bind uniform block to binding point
	glUniformBlockBinding(shaderProgram, blockIndex, bindingPoint);
}

} // end R308 namespace