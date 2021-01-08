#include "playground.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>

#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

#include <Windows.h>
#include <mmsystem.h>

#include "PerlinNoise.hpp"

// Initialize window and game loop
int main(void)
{
	// Initialize window
	bool windowInitialized = initializeWindow();
	if (!windowInitialized) return -1;

	// Initialize vertex buffer
	bool vertexbufferInitialized = initializeVertexbuffer();
	if (!vertexbufferInitialized) return -1;

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Get a handle for our "MVP" and "MV" uniforms and update them for initalization 
	MatrixIDMV = glGetUniformLocation(programID, "MV");
	MatrixID = glGetUniformLocation(programID, "MVP");
	timeID = glGetUniformLocation(programID, "iTime");

	updateMVPTransformation();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Timing for varying colors
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	float secondsPassedOld = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0f;
	secondsPassedOld = static_cast<float>(static_cast<int>(secondsPassedOld * 100.)) / 100.;
	float secondsPassed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0f;
	secondsPassed = static_cast<float>(static_cast<int>(secondsPassed * 100.)) / 100.;

	// Start animation loop until escape key is pressed
	do {
		update();

		//if (secondsPassed != secondsPassedOld)
		//	std::cout << "Time difference = " << secondsPassed << " [s]" << std::endl;

		// Time passed
		secondsPassedOld = secondsPassed;
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		secondsPassed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() / 1000.0f;
		secondsPassed = static_cast<float>(static_cast<int>(secondsPassed * 100.)) / 100.;

		// Send passed time to fragmentshader
		glUniform1f(timeID, secondsPassed);

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup and close window
	cleanupVertexbuffer();
	glDeleteProgram(programID);
	closeWindow();

	return 0;
}

// Decides what happens in each frame
void update()
{
	// Time logic
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Clear the screen, Background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.42f, 0.52f, 0.0f);

	// Use our shader
	glUseProgram(programID);

	// Player controls
	processPlayerInput();

	// Update the MVP transformation with the new values
	updateMVPTransformation();

	// Send our transformation to the currently bound shader, in the "MVP" uniform and also the "MV" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(MatrixIDMV, 1, GL_FALSE, &MV[0][0]);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
	glVertexAttribPointer(
		1,                  // attribute 1. No particular reason for 1, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Draw the triangle
	glDrawArrays(GL_TRIANGLES, 0, vertexbuffer_size); // 3 indices starting at 0 -> 1 triangle

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void generateMap(std::vector< glm::vec3 >& vertices, std::vector< glm::vec3 >& normals) {
	placeCube(vertices, normals, 0, 0, 0);

	double frequency = 3; //value between 1 and 64
	std::int32_t octaves = 8; //value between 1 and 16
	std::uint32_t seed = 123;

	float heightStretch = 10;

	int fillDepth = -4;

	const int width = 60;
	const int depth = 60;
	const double fx = width / frequency;
	const double fy = depth / frequency;

	const siv::PerlinNoise perlin(seed); //new perlin noise object with seed
	

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < depth; y++) {
			//get height for coordinate
			int height = static_cast<int>(perlin.accumulatedOctaveNoise2D(x / fx, y / fy, octaves) * heightStretch);

			if (height < -3) {
				height = -3;
			}

			//output to console
			//std::cout << height << " | " ;

			//place cube
			placeCube(vertices, normals, x, y, height);
			
			//fill down to -10
			for (int i = height - 1; i > fillDepth; i--) {
				placeCube(vertices, normals, x, y, i);
			}
		}
		std::cout << std::endl;
	}
}

// Process User Input
void processPlayerInput() {
	float cameraSpeed = 30.0 * deltaTime;
	float multiplicator = 1.0;

	// Shift key for faster camera
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		multiplicator = 3.0;
	}


	// Camera angle
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront * multiplicator;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront * multiplicator;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * multiplicator;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * multiplicator;

}


// Window initialization
bool initializeWindow()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return false;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGR_Project", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return false;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Mouse capturing
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	return true;
}

// Updates the MVP matrix according to userInput
bool updateMVPTransformation()
{
	// Model (World) matrix
	glm::mat4 Model = glm::mat4(1.0f); // Identity matrix
	Model = glm::rotate(Model, angle_maze, glm::vec3(1.0f, 0.0f, 0.0f)); // Apply orientation (last parameter: axis)
	Model = glm::translate(Model, glm::vec3(0, 0, 0)); // Apply translation

	// View (Camera) matrix (modify this to let the camera move)
	glm::mat4 View = glm::lookAt(
		cameraPos,
		cameraPos + cameraFront,
		cameraUp
	);

	// Projection matrix : 80° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(fov), 4.0f / 3.0f, 0.1f, 10000.0f);

	// MVP
	MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around
	MV = View * Model; // We also need MV in the shader to transform the light position

	return true;
}


// Callback function for mouse events
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}


// Initialize buffers for vertices
bool initializeVertexbuffer()
{
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create vertex and normal data
	std::vector< glm::vec3 > vertices = std::vector< glm::vec3 >();
	std::vector< glm::vec3 > normals = std::vector< glm::vec3 >();
	generateMap(vertices, normals);

	vertexbuffer_size = vertices.size() * sizeof(glm::vec3);

	glGenBuffers(2, vertexbuffer); // Generate two buffers, one for the vertices, one for the normals

	// Fill first buffer (vertices)
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Fill second buffer (normals)
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	return true;
}

// Resource cleanup on exit
bool cleanupVertexbuffer()
{
	// Cleanup VBO
	glDeleteVertexArrays(1, &VertexArrayID);
	return true;
}


// Handle closing of window
bool closeWindow()
{
	glfwTerminate();
	return true;
}

// Loads a 3D object in an STL file
void loadSTLFile(std::vector< glm::vec3 >& vertices, std::vector< glm::vec3 >& normals, std::string stl_file_name)
{
	// parse stl file
	stl::stl_data info = stl::parse_stl(stl_file_name);

	//std::vector<stl::triangle> triangles = info.triangles;

	//iterate over triangles
	for (int i = 0; i < info.triangles.size(); i++) {
		stl::triangle t = info.triangles.at(i);
		glm::vec3 triangleNormal = glm::vec3(t.normal.x,
			t.normal.y,
			t.normal.z);
		//add vertex and normal for point 1:
		vertices.push_back(glm::vec3(t.v1.x, t.v1.y, t.v1.z));
		normals.push_back(triangleNormal);
		//add vertex and normal for point 2:
		vertices.push_back(glm::vec3(t.v2.x, t.v2.y, t.v2.z));
		normals.push_back(triangleNormal);
		//add vertex and normal for point 3:
		vertices.push_back(glm::vec3(t.v3.x, t.v3.y, t.v3.z));
		normals.push_back(triangleNormal);
	}
}


void placeCube(std::vector< glm::vec3 >& vertices, std::vector< glm::vec3 >& normals, float posX, float posY, float posZ) {

	// parse stl file
	stl::stl_data info = stl::parse_stl("cube.stl");

	//std::vector<stl::triangle> triangles = info.triangles;

	//iterate over triangles
	for (int i = 0; i < info.triangles.size(); i++) {
		stl::triangle t = info.triangles.at(i);
		glm::vec3 triangleNormal = glm::vec3(t.normal.x,
			t.normal.y,
			t.normal.z);
		//add vertex and normal for point 1:
		vertices.push_back(glm::vec3(t.v1.x + posX, t.v1.y + posY, t.v1.z + posZ));
		normals.push_back(triangleNormal);
		//add vertex and normal for point 2:
		vertices.push_back(glm::vec3(t.v2.x + posX, t.v2.y + posY, t.v2.z + posZ));
		normals.push_back(triangleNormal);
		//add vertex and normal for point 3:
		vertices.push_back(glm::vec3(t.v3.x + posX, t.v3.y + posY, t.v3.z + posZ));
		normals.push_back(triangleNormal);
	}

}


// Handle function for window resizing
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}