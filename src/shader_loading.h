#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

struct Shader {
	GLuint id = 0;

	bool loadShaderProgramFromFile(const char* vertexShaderPath, const char* fragmentShaderPath);

	bool loadShaderProgramFromData(const char* vertexShaderData, const char* fragmentShaderData);

	void bind();

	void clear();

	GLuint getId() const { return id; }
};

static GLuint shaderId;

GLint createShaderFromData(const char* data, GLenum shaderType) {

	shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &data, nullptr);
	glCompileShader(shaderId);

	GLint result = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);

	if (!result) {
		char* message = 0;
		int l = 0;

		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &l);

		if (l) {
			message = new char[l];

			glGetShaderInfoLog(shaderId, l, &l, message);

			message[l - 1] = 0;

			std::cout << data << ":\n" << message << "\n";

			delete[] message;
		}
		else {
			std::cout << "\nUnknown error\n";
		}

		glDeleteShader(shaderId);

		shaderId = 0;
	}

	return shaderId;
}

char* readEntireFile(const char* source) {
	std::ifstream file;
	file.open(source);

	if (!file.is_open()) {
		std::cout << "Error loading file: " << source << "\n";
		return nullptr;
	}

	GLint size = 0;
	file.seekg(0, file.end);
	size = file.tellg();
	file.seekg(0, file.beg);

	char* fileContent = new char[size + 1] {};

	file.read(fileContent, size);

	file.close();

	return fileContent;
}

bool Shader::loadShaderProgramFromFile(const char* vertexShaderPath, const char* fragmentShaderPath) {

	char* vertexData = readEntireFile(vertexShaderPath);
	char* fragmentData = readEntireFile(fragmentShaderPath);

	if (vertexData == nullptr || fragmentData == nullptr) {
		delete[] vertexData;
		delete[] fragmentData;

		return 0;
	}

	bool res = loadShaderProgramFromData(vertexData, fragmentData);

	delete[] vertexData;
	delete[] fragmentData;

	return res;
}

bool Shader::loadShaderProgramFromData(const char* vertexShaderData, const char* fragmentShaderData) {

	auto vertexId = createShaderFromData(vertexShaderData, GL_VERTEX_SHADER);

	if (vertexId == 0) {
		return false;
	};

	auto fragmentId = createShaderFromData(fragmentShaderData, GL_FRAGMENT_SHADER);

	if (fragmentId == 0) {
		glDeleteShader(vertexId);
		return 0;
	}

	id = glCreateProgram();
	glAttachShader(id, vertexId);
	glAttachShader(id, fragmentId);

	glLinkProgram(id);

	glDeleteShader(vertexId);
	glDeleteShader(fragmentId);

	GLint info = 0;
	glGetProgramiv(id, GL_LINK_STATUS, &info);

	if (info != GL_TRUE) {
		char* message = 0;
		
		int l = 0;

		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &l);

		message = new char[l];

		glGetProgramInfoLog(id, l, &l, message);

		message[l - 1] = 0;

		std::cout << "Link error: " << message << "\n";

		delete[] message;

		glDeleteProgram(id);
		id = 0;
		return 0;
	}

	glValidateProgram(id);
	return true;
}

void Shader::bind() {
	glUseProgram(id);
}

void Shader::clear() {
	glDeleteProgram(id);
	*this = {};
}