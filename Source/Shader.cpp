#include "Shader.hpp"

Shader::Shader(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	shaderID = CreateShader(vertexShaderFilename, fragmentShaderFilename);
}

Shader::~Shader()
{
	glDeleteProgram(shaderID);
}

void Shader::Bind()
{
	glUseProgram(shaderID);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

GLuint Shader::Compile(std::string shaderSource, GLenum type)
{
	// Creates the shader for the desired type (vertex or fragment)t)

	GLuint id = glCreateShader(type);
	const char* src = shaderSource.c_str();

	glShaderSource(id, 1, &src, 0);
	glCompileShader(id);

	// Handles compile errors
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE)
	{
		int length = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Shader compilation error: " << message << std::endl;
		delete[] message;
		return 0;
	}

	return id;
}

std::string Shader::Parse(const char* filename)
{
	// Reads the shader files and returns their content

	FILE* file;
	if (fopen_s(&file, filename, "rb") != 0) {
		std::cout << "File " << filename << " not found" << std::endl;
		return "";
	}

	std::string contents;
	fseek(file, 0, SEEK_END);
	size_t filesize = ftell(file);
	rewind(file);
	contents.resize(filesize);

	fread(&contents[0], 1, filesize, file);
	fclose(file);

	return contents;
}

GLuint Shader::CreateShader(const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	std::string vertexShaderSource = Parse(vertexShaderFilename);
	std::string fragmentShaderSource = Parse(fragmentShaderFilename);

	GLuint program = glCreateProgram();
	GLuint vertexShader = Compile(vertexShaderSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = Compile(fragmentShaderSource, GL_FRAGMENT_SHADER);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	return program;
}

GLuint Shader::GetShaderID()
{
	return shaderID;
}
