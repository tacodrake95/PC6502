#ifndef GFX_H
#define GFX_H

#include <string>
#define GLEW_STATIC

#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "GL/freeglut.h"

class gfx
{
private:
	GLFWwindow * window;
	GLuint VBO, VAO, tex;
	int shaderProgram;
	static GLuint loadShader(uint8_t shadertype, std::string filename);
	//uint8_t * pixels;
public:
	enum { RED = 0, GREEN = 1, BLUE = 2};
	static void GenData(GLuint &VBO, GLuint &VAO);
	void GenTex(uint8_t &pixels);
	void UpdateTex(uint8_t &pixels, uint16_t format, uint16_t xRes, uint16_t yRes);
	void ProcessInput();
	void Render(uint8_t &pixels, uint8_t mode);
	static GLint PrepareShader(std::string vertFilename, std::string fragFilename);
	bool shouldWindowClose();
	gfx(uint8_t version, uint8_t subVersion);
	~gfx();
};

#endif