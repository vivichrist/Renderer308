#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <string>

namespace R308
{

class ShaderStorage
{
public:
	ShaderStorage( GLuint binding, uint size );
	~ShaderStorage();
	void setStorageDataf(GLfloat const *data, uint const &col, uint const &row, uint const &offset);
	void setStorageDatai(GLint const *data, uint const &col, uint const &row, uint const &offset);
	void setStorageDataui(GLuint const *data, uint const &col, uint const &row, uint const &offset);
	void setStorageDataArray(const uint &offset, const T *data, const size_t &size) const;
	void bindShaderStorage(const GLuint &shaderProgram, const std::string &block);

      private:
	uint bufferSize;
	GLuint buffer;
	GLuint bindingPoint;
};

}