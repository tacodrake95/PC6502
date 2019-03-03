#include <iostream>
#include <fstream>
#include <string>

#include "gfx.h"

long GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

gfx::gfx(uint8_t version, uint8_t subVersion)
{
	glfwInit();
	window = glfwCreateWindow(1024, 768, "LEXIPIXEL", nullptr, nullptr); // Windowed
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, subVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);
	glViewport(0, 0, 1024, 768);
	glewExperimental = GL_TRUE;
	glewInit();

	shaderProgram = PrepareShader("./shaders/tri.vert", "./shaders/pix.frag");
	GenData(VBO, VAO);
}

gfx::~gfx()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
}

GLuint gfx::loadShader(uint8_t shadertype, std::string filename)
{

	unsigned int shaderOut;
	switch (shadertype)
	{
	case 0:
		shaderOut = glCreateShader(GL_VERTEX_SHADER);
		break;
	case 1:
		shaderOut = glCreateShader(GL_FRAGMENT_SHADER);
		break;
	default:
		std::cout << "INCORRECT SHADER TYPE\n";
	}

	long fileSize = GetFileSize(filename) + 1; //tested filesize function elsewhere in program

	char * shaderSource = new char[fileSize];
	std::ifstream inFile(filename, std::ios::in | std::ios::binary);
	inFile.read(shaderSource, fileSize);
	inFile.close();
	shaderSource[fileSize - 1] = '\0';

	glShaderSource(shaderOut, 1, &shaderSource, NULL);
	glCompileShader(shaderOut);
	delete[] shaderSource;

	int success;

	glGetShaderiv(shaderOut, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar * infoLog = new GLchar[512];
		glGetShaderInfoLog(shaderOut, 512, NULL, infoLog);
		std::cout << "ERROR: " << std::hex << (int)shadertype << " SHADER COMPILATION FAILED" << infoLog << "\n";
		delete[] infoLog;
		return NULL;
	}
	return shaderOut;
}

GLint gfx::PrepareShader(std::string vertFilename, std::string fragFilename)
{
	unsigned int vertexShader = loadShader(0, vertFilename);
	unsigned int fragmentShader = loadShader(1, fragFilename);
	GLint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

void gfx::ProcessInput()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void gfx::GenData(GLuint &VBO, GLuint &VAO)
{

	float vertices[] = 
	{
		 1,  1, 0,  // top right
		 1, -1, 0,  // bottom right
		-1,  1, 0,
		 1, -1, 0,
		-1, -1, 0,  // bottom left
		-1,  1, 0   // top left
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);


	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

bool gfx::shouldWindowClose()
{
	return glfwWindowShouldClose(window);
}

void gfx::Render(uint8_t &pixels, uint8_t mode)
{
	uint16_t xRes;
	uint16_t yRes;
	uint16_t format;

	uint8_t res = (mode & 0x0C) >> 2;

	// Set Resolution
	switch (res)
	{
	case 0:
		xRes = 320;
		yRes = 200;
		break;
	case 1:
		xRes = 640;
		yRes = 480;
		break;
	case 2:
		xRes = 800;
		yRes = 600;
		break;
	case 3:
		xRes = 1024;
		yRes = 768;
	}

	// Set Data Format
	uint8_t fmt = (mode & 0x03);
	switch (fmt)
	{
	case 0:
		format = GL_R3_G3_B2;
		break;
	case 1:
		format = GL_RGB565;
		break;
	case 2:
		format = GL_RGB8;
		break;
	case 3:
		//format = GL_RGBA8;
		format = GL_RGB8;
	}

	// Check total size of data
	UpdateTex(pixels, format, xRes, yRes);
	
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_2D, tex);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void gfx::GenTex(uint8_t &pixels)
{
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, &pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void gfx::UpdateTex(uint8_t &pixels, uint16_t format, uint16_t xRes, uint16_t yRes)
{
	uint16_t type;
	switch (format)
	{
	case GL_RGB8:
	case GL_RGBA8:
		type = GL_UNSIGNED_BYTE;
		break;
	case GL_RGB5_A1:
		type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
		break;
	case GL_RGB565:
		type = GL_UNSIGNED_SHORT_5_6_5;
		break;
	case GL_R3_G3_B2:
		type = GL_UNSIGNED_BYTE_3_3_2;
		break;
	default:
		return;
	}
	glBindTexture(GL_TEXTURE_2D, tex);
	//glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, format, xRes, yRes, 0, GL_RGB, type, &pixels);
	glBindTexture(GL_TEXTURE_2D, 0);
}