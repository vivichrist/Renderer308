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
	template<typename T>
	void setStorageData(T const *data, uint const &col, uint const &row, uint const &offset);
	template<typename T>
	void setStorageDataArray(T const* data, uint const& offset, size_t const& size);
	void bindShaderStorage(const GLuint &shaderProgram, const std::string &block);

private:
	uint bufferSize;
	GLuint buffer;
	GLuint bindingPoint;
};

}