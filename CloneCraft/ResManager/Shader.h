#pragma once

#include <string>

#include <glad\glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader() { }
	// Sets the current shader as active
	Shader  &use();
	// Compiles the shader from given source code
	void    compile(const GLchar *vertexSource, const GLchar *fragmentSource, const GLchar *geometrySource = nullptr);

	void    setFloat(const GLchar *name, GLfloat value);
	void    setInt(const GLchar *name, GLint value);
	void    setVec2(const GLchar *name, const glm::vec2 &value);
	void    setVec3(const GLchar *name, const glm::vec3 &value);
	void    setVec4(const GLchar *name, const glm::vec4 &value);
	void    setMat4(const GLchar *name, const glm::mat4 &matrix);

	GLuint	getId();
private:
	// Checks if compilation or linking failed and if so, print the error logs
	GLuint	m_id;
	void    checkCompileErrors(GLuint object, std::string type);
};