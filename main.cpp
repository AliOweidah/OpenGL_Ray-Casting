// include C++ headers
#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// inculde shader
#include "ShaderProgram.h"


//define screen size
unsigned int SCREEN_WIDTH = 800;
unsigned int SCREEN_HEIGHT = 800;


struct VertexColor // vertex attribute format
{
	GLfloat position[3];
	GLfloat color[3];
};


// scene content
ShaderProgram gShader;	// shader program object
GLuint gVBO = 0;		// vertex buffer object identifier
GLuint gIBO = 0;		// index buffer object identifier
GLuint gVAO = 0;		// vertex array object identifier


std::map <std::string, glm::mat4> gModelMatrix;	// object matrices
glm::mat4 gViewMatrix;			// view matrix
glm::mat4 gProjectionMatrix;	// projection matrix


static void init(GLFWwindow* window) 
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // set the color the color buffer should be cleared to

	glEnable(GL_DEPTH_TEST);	// enable depth buffer test

	gShader.compileAndLink("modelViewProj.vert", "color.frag"); // compile and link a vertex and fragment shader pair

	gViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // initialise view matrix

	float aspectRatio = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT; // initialise projection matrix

	gProjectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10.0f);

	gModelMatrix["cube"] = glm::mat4(1.0f);

	std::vector<GLfloat> vertices =
	{
		// colour cube
		-0.5f, 0.5f, 0.5f,	// vertex 0: position
		1.0f, 0.0f, 1.0f,	// vertex 0: colour
		-0.5f, -0.5f, 0.5f,	// vertex 1: position
		1.0f, 0.0f, 0.0f,	// vertex 1: colour
		0.5f, 0.5f, 0.5f,	// vertex 2: position
		1.0f, 1.0f, 1.0f,	// vertex 2: colour
		0.5f, -0.5f, 0.5f,	// vertex 3: position
		1.0f, 1.0f, 0.0f,	// vertex 3: colour
		-0.5f, 0.5f, -0.5f,	// vertex 4: position
		0.0f, 0.0f, 1.0f,	// vertex 4: colour
		-0.5f, -0.5f, -0.5f,// vertex 5: position
		0.0f, 0.0f, 0.0f,	// vertex 5: colour
		0.5f, 0.5f, -0.5f,	// vertex 6: position
		0.0f, 1.0f, 1.0f,	// vertex 6: colour
		0.5f, -0.5f, -0.5f,	// vertex 7: position
		0.0f, 1.0f, 0.0f,	// vertex 7: colour
	};

	// colour cube indices
	std::vector<GLuint> indices = {
		0, 1, 2,	// triangle 1
		2, 1, 3,	// triangle 2
		4, 5, 0,	// triangle 3
		0, 5, 1,	// triangle 4
		2, 3, 6,	// triangle 5
		6, 3, 7,	// triangle 6	
		4, 0, 6,	// triangle 7
		6, 0, 2,	// triangle 8
		1, 5, 3,	// triangle 9
		3, 5, 7,	// triangle 10
		5, 4, 7,	// triangle 11
		7, 4, 6,	// triangle 12
	};
	// create VBO
	glGenBuffers(1, &gVBO);					// generate unused VBO identifier
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// generate identifier for IBO and copy data to GPU
	glGenBuffers(1, &gIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// create VAO, specify VBO data and format of the data
	glGenVertexArrays(1, &gVAO);			// generate unused VAO identifier
	glBindVertexArray(gVAO);				// create VAO
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);	// bind the VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor),
		reinterpret_cast<void*>(offsetof(VertexColor, position)));	// specify format of position data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexColor),
		reinterpret_cast<void*>(offsetof(VertexColor, color)));		// specify format of colour data

	glEnableVertexAttribArray(0);	// enable vertex attributes
	glEnableVertexAttribArray(1);
}


static void update_scene(GLFWwindow* window)
{
	static float rotationAngle = 0.0f;
	rotationAngle += 0.016f;
	gModelMatrix["cube"] = glm::rotate(rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Update model matrix
}


static void render_scene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clean color and depth buffers

	gShader.use();  // Use the shader program

	glBindVertexArray(gVAO);	// make VAO active

	glm::mat4 MVP;

	MVP = gProjectionMatrix * gViewMatrix * gModelMatrix["cube"];
	gShader.setUniform("uModelViewProjectionMatrix", MVP);  // set Uniform variable
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);	// display vertices base indices and primitive type

	glFlush(); //flush the graphic pipeline
}

static void error_callback(int error, const char* error_description) {
	std::cerr << error_description << std::endl; // show error description
}

int main(void)
{
	GLFWwindow* app_window = nullptr; // Define application window

	glfwSetErrorCallback(error_callback); // Set GLFW error callback function

	if (!glfwInit())
	{
		exit(EXIT_FAILURE); //if GLFW failed to initialise -> Exit
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	app_window = glfwCreateWindow(SCREEN_HEIGHT, SCREEN_HEIGHT, "OpenGL Cube", nullptr, nullptr); // Create the window

	if (app_window == nullptr) // Check if app_window is created successfully
	{
		glfwTerminate();
		exit(EXIT_FAILURE); // "exit(EXIT_FAILURE)" can also be written as "(return -1)"
	}

	glfwMakeContextCurrent(app_window); //  set window context as current context
	glfwSwapInterval(1);				//	swap buffer interval

	if (glewInit() != GLEW_OK) // if GLEW failed to initialise -> Exit
	{
		std::cerr << "GLEW Failed to initialise" << std::endl;
		exit(EXIT_FAILURE);
	}

	init(app_window); // initialise scene and render settings

	while (!glfwWindowShouldClose(app_window)) 	// the rendering loop
	{
		update_scene(app_window); // update the scene

		render_scene(); // render the scene

		glfwSwapBuffers(app_window);
		glfwPollEvents();


	
	}
	// Clean ups

	glDeleteBuffers(1, &gVBO);
	glDeleteBuffers(1, &gIBO);
	glDeleteVertexArrays(1, &gVAO);

	glfwDestroyWindow(app_window);
	glfwTerminate();

	exit(EXIT_SUCCESS);

}