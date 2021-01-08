#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>

#include <vector>
#include <playground/parse_stl.h>

#define M_PI 3.14159265358979323846

// ------------------------------------- ---------- ------------------------------------- //

/// <summary>
///		Window
/// </summary>
GLFWwindow* window;

/// <summary>
///		Width of the window
/// </summary>
const unsigned int SCR_WIDTH = 1024;

/// <summary>
///		Height of the window
/// </summary>
const unsigned int SCR_HEIGHT = 768;

/// <summary>
///		Vertex Buffer
/// </summary>
GLuint vertexbuffer[2];

/// <summary>
///		ID of the vertex array
/// </summary>
GLuint VertexArrayID;

/// <summary>
///		Size of the vertex buffer
/// </summary>
GLuint vertexbuffer_size;

/// <summary>
///		Shader Program ID
/// </summary>
GLuint programID;

// ------------------------------------- Identifiers ------------------------------------- //

/// <summary>
///		MVP Matrix ID
/// </summary>
GLuint MatrixID;

/// <summary>
///		MVP Matrix
/// </summary>
glm::mat4 MVP;

/// <summary>
///		MV Matrix ID
/// </summary>
GLuint MatrixIDMV;

/// <summary>
///		MV Matrix
/// </summary>
glm::mat4 MV;

/// <summary>
///		Time ID
/// </summary>
GLuint timeID;

/// <summary>
///		Boolean to enable cheat mode
/// </summary>
bool allowCheats = false;

/// <summary>
///		Boolean to allow proper cheat mode toggle
/// </summary>
bool allowXInput = true;

/// <summary>
///		Timer when cheat mode can be en-/disabled
/// </summary>
float allowTimer = 1.0f;

/// <summary>
///		Time between current and last frame
/// </summary>
float deltaTime = 0.0f;

/// <summary>
///		Time of last frame
/// </summary>
float lastFrame = 0.0f;

// ------------------------------------- Camera (Player) Transform ------------------------------------- //

/// <summary>
///		X position of the player
/// </summary>
float curr_x = -12.0f;

/// <summary>
///		Y position of the player
/// </summary>
float curr_y = 16.0f;

/// <summary>
///		Z position of the player
/// </summary>
float curr_z = 55.0f;

/// <summary>
///		Player position vector for lookAt method
/// </summary>
glm::vec3 cameraPos = glm::vec3(curr_x, curr_y, curr_z);

/// <summary>
///		Camera direction for lookAt method
/// </summary>
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

/// <summary>
///		Camera alignment for lookAt method
/// </summary>
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// ------------------------------------- Maze Transform ------------------------------------- //

/// <summary>
///		Maze angle after loading the stl file
/// </summary>
float angle_maze = -0.5f * (float) M_PI;

// ------------------------------------- Mouse Input ------------------------------------- //

/// <summary>
///		Allow mouse input
/// </summary>
bool firstMouse = true;

/// <summary>
///		Yaw (initialized to -90.0 degrees since a yaw of 0.0 results 
///		in a direction vector pointing to the right so we initially rotate a bit to the left
/// </summary>
float yaw = -90.0f;

/// <summary>
///		Pitch
/// </summary>
float pitch = 0.0f;

/// <summary>
///		Initial mouse x position
/// </summary>
float lastX = SCR_WIDTH / 2.0;

/// <summary>
///		Initial mouse y position
/// </summary>
float lastY = SCR_HEIGHT / 2.0;

/// <summary>
///		Camera field of view
/// </summary>
float fov = 90.0f;

// ------------------------------------- Methods ------------------------------------- //

/// <summary>
///		Initialize window and game loop
/// </summary>
/// <param name="">Arguments</param>
/// <returns>Exit code</returns>
int main(void);

/// <summary>
///		Decides what happens in each frame
/// </summary>
void update();

/// <summary>
///		Process User Input
/// </summary>
void processPlayerInput();

/// <summary>
///		Callback function for mouse events
/// </summary>
/// <param name="window">Window</param>
/// <param name="xpos">x</param>
/// <param name="ypos">y</param>
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

/// <summary>
///		Window initialization
/// </summary>
/// <returns>True, if window was successfully initialized</returns>
bool initializeWindow();

/// <summary>
///		Updates the MVP matrix according to userInput
/// </summary>
/// <returns>Error code</returns>
bool updateMVPTransformation();

/// <summary>
///		Initialize buffers for vertices
/// </summary>
/// <returns></returns>
bool initializeVertexbuffer();

/// <summary>
///		Resource cleanup on exit
/// </summary>
/// <returns>Error code</returns>
bool cleanupVertexbuffer();

/// <summary>
///		Handle closing of window
/// </summary>
/// <returns>Error code</returns>
bool closeWindow();

/// <summary>
///		Handle function for window resizing
/// </summary>
/// <param name="window">Window</param>
/// <param name="width">Width</param>
/// <param name="height">Height</param>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/// <summary>
///		Loads a STL file and converts it to a vector of vertices and normal
/// </summary>
/// <param name="vertices">Vector of vertices, needs to be empty and is filled by the function</param>
/// <param name="normals">Vector of normals, needs to be empty and is filled by the function</param>
/// <param name="stl_file_name">File name of the STL file that should be loaded</param>
void loadSTLFile(std::vector< glm::vec3 >& vertices, std::vector< glm::vec3 >& normals, std::string stl_file_name);


void placeCube(std::vector< glm::vec3 >& vertices, std::vector< glm::vec3 >& normals, float posX, float posY, float posZ);

void generateMap(std::vector< glm::vec3 >& vertices, std::vector< glm::vec3 >& normals);

#endif
