#pragma comment (lib, "glew32s.lib")
#define GLEW_STATIC
#include <stdio.h>
#include <string.h>
#include <cmath>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VBO, VAO, shader, uniformModel;

int direction = 0;
float triOffsetX = 0.0f;
float triOffsetY = 0.0f;
float triMaxOffsetPos = 0.75f;
float triMaxOffsetMin = 0.0f;
float triIncrement = 0.0005f;

//scalling
bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.9f;
float minSize = 0.2f;

// Vertex Shader code
static const char* vShader = "                                                \n\
#version 460                                                                  \n\
                                                                              \n\
layout (location = 0) in vec3 pos;											  \n\
                                                                              \n\
uniform mat4 model;                                                          \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    gl_Position = model * vec4(pos, 1.0);		  \n\
}";

// Fragment Shader
static const char* fShader = "                                                \n\
#version 460                                                                  \n\
                                                                              \n\
out vec4 colour;                                                               \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    colour = vec4(1.5, 0.5, 1.0, 1.0);                                         \n\
}";

void CreateTriangle()
{
	GLfloat vertices[] = {
		-2.0f, -2.0f, 0.0f, // Left
		-1.0f, -2.0f, 0.0f, // Right
		-1.5f, -1.0f, 0.0f,  // Top
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, 1024, NULL, eLog);
		fprintf(stderr, "Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	shader = glCreateProgram();

	if (!shader)
	{
		printf("Failed to create shader\n");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
}

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW initialisation failed!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow Forward Compatbility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the window
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get Buffer Size information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow))
	{
		// Get + Handle user input events
		glfwPollEvents();
		// Clear window
		glClearColor(0.5f, 0.0f, 2.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model(1.0f);

		/*if (direction == 0)
		{
			model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
			triOffset += triIncrement;
			direction++;
		}
		if (direction == 1)
		{
			model = glm::translate(model, glm::vec3(0.0f, triOffset, 0.0f));
			triOffset += triIncrement;
			direction++;
		}
		if (direction == 2)
		{
			model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
			triOffset -= triIncrement;
			direction++;
		}
		if (direction == 3)
		{
			model = glm::translate(model, glm::vec3(0.0f, triOffset, 0.0f));
			triOffset -= triIncrement;
			direction = 0;
		}*/

		//if (direction == 0)
		//{
		//	model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));
		//	triOffset += triIncrement;
		//	//direction = 1;
		//	if (triOffset >= triMaxOffset)
		//	{
		//		//triOffset = triMaxOffset;
		//		direction = 1;
		//	}
		//	//direction = 1;
		//}
		//else if (direction == 1)
		//{
		//	model = glm::translate(model, glm::vec3(0.0f, triOffset, 0.0f));
		//	triOffset += triIncrement;
		//	if (triOffset >= triMaxOffset)
		//	{
		//		direction = 2;
		//		//triOffset = triMaxOffset;
		//	}
		//	//direction++;
		//}
		//else if (direction == 2)
		//{
		//	model = glm::translate(model, glm::vec3(-triOffset, 0.0f, 0.0f));
		//	triOffset -= triIncrement;
		//	if (triOffset <= -triMaxOffset)
		//	{
		//		direction = 3;
		//		triOffset = -triMaxOffset;
		//	}
		//	//direction++;
		//}
		//else if (direction == 3)
		//{
		//	model = glm::translate(model, glm::vec3(0.0f, -triOffset, 0.0f));
		//	triOffset -= triIncrement;
		//	if (triOffset <= triMaxOffset)
		//	{
		//		direction = 0;
		//		triOffset = 0.0f;
		//	}
		//	//direction = 0;
		//}

		if (direction == 0)
		{
			model = glm::translate(model, glm::vec3(triOffsetX, triOffsetY, 0.0f));
			triOffsetX += triIncrement;
			if (triOffsetX >= triMaxOffsetPos)
			{
				direction = 1;
				triOffsetX = triMaxOffsetPos;
			}
		}
		else if (direction == 1)
		{
			model = glm::translate(model, glm::vec3(triOffsetX, triOffsetY, 0.0f));
			triOffsetY += triIncrement;
			if (triOffsetY >= triMaxOffsetPos)
			{
				direction = 2;
				triOffsetY = triMaxOffsetPos;
			}
		}
		else if (direction == 2)
		{
			model = glm::translate(model, glm::vec3(triOffsetX, triOffsetY, 0.0f));
			triOffsetX -= triIncrement;
			if (triOffsetX <= triMaxOffsetMin)
			{
				direction = 3;
				triOffsetX = triMaxOffsetMin;
			}
		}

		else if (direction == 3)
		{
			model = glm::translate(model, glm::vec3(triOffsetX, triOffsetY, 0.0f));
			triOffsetY -= triIncrement;
			if (triOffsetY <= triMaxOffsetMin)
			{
				direction = 0;
				triOffsetY = triMaxOffsetMin;
			}
		}

		/*if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}*/

		//// Clear window
		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		//glUseProgram(shader);

		/*glm::mat4 model(1.0f);*/

		//translasi
		//model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));

		//scalling
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));



		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));


		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}

	return 0;
}