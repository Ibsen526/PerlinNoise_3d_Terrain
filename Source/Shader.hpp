#include <GL/glew.h>
#include <iostream>
#include <string>
#include <fstream>

class Shader
{
public:
	Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename);
	virtual ~Shader();

	void Bind();
	void Unbind();

	GLuint GetShaderID();

private:
	GLuint Compile(std::string shaderSource, GLenum type);
	std::string Parse(const char* filename);
	GLuint CreateShader(const char* vertexShaderFilename, const char* fragmentShaderFilename);

	GLuint shaderID;
};
