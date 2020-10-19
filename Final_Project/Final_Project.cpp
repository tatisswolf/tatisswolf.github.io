/*
 * Final_Project.cpp
 *
 *  Created on: Dec 15, 2019
 *      Author: Jordan Tatis
 */




// Header inclusions
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM math header inclusions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std; // standard namespace.

#define WINDOW_TITLE "Final Project Jordan Tatis" // Window title macro.

// shader program macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// variables declarations for shader, window size initialization, buffer and array objects
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO;

GLfloat cameraSpeed = 0.0005f; // Movement speed per frame

GLchar currentKey; // Stores key pressed
GLchar currentKey2; // Stores a second key pressed.

GLfloat lastMouseX = 400, lastMouseY = 300; // Locks mouse cursor at the center of the screen
GLfloat mouseXOffset, mouseYOffset, yaw = 0.0f, pitch = 0.0f; // mouse offset, yaw, and pitch variables
GLfloat sensitivity = 0.5f; // used for mouse / camera rotation sensitivity
bool mouseDetected = true; // initially true when mouse movement is detected.

// Global vector declarations
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f); // Initial camera position. Placed 5 units in Z
glm::vec3 CameraUpY = glm::vec3(0.0f, 1.0f, 0.0f); // Temporary y unit vector.
glm::vec3 CameraForwardZ = glm::vec3(0.0f, 0.0f, -1.0f); // Temporary z unit vector
glm::vec3 CameraUpX = glm::vec3(0.0f, 1.0f, 1.0f);
glm::vec3 front; // temporary z unit vector for mouse

// Function prototypes
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UKeyboard(unsigned char key, int x, int y);
void UKeyReleased(unsigned char key, int x, int y);
void UMouseMove(int x, int y);

// Vertex shader source code
const GLchar * vertexShaderSource = GLSL(330,
		layout (location = 0) in vec3 position; //vertex data from vertex Attrib Pointer 0
		layout (location = 1) in vec3 color; // Color data from vertex Attrib Pointer 1

		out vec3 mobileColor; // Variable to transfer color data to the fragment shader.

		// Global variables for the transform matrices
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

	void main(){
		gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertex to clip coordinates
		mobileColor = color; // References incoming color data.
	}
);

// Fragment shader source code
const GLchar * fragmentShaderSource = GLSL(330,
		in vec3 mobileColor; // variable to hold incoming color data from vertex shader

		out vec4 gpuColor; // Variable to pass color data to the GPU

	void main(){

		gpuColor = vec4(mobileColor, 1.0); // Sends color data to the GPU for rendering

	}
);

// Main program
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	glutReshapeFunc(UResizeWindow);


	glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			std::cout << "Failed to initialize GLEW" << std::endl;
			return -1;
		}

	UCreateShader();

	UCreateBuffers();

	// Use the shader program
	glUseProgram(shaderProgram);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Sets background color

	glutDisplayFunc(URenderGraphics);

	glutKeyboardFunc(UKeyboard); // detects key pressed.

	glutKeyboardUpFunc(UKeyReleased); // Detects key release

	glutPassiveMotionFunc(UMouseMove); // Detects mouse movement

	glutMainLoop();

	// Destroys buffer objects once used.
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;

}

// Resizes the window
void UResizeWindow(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

// Renders graphics
void URenderGraphics(void)
{
	glEnable(GL_DEPTH_TEST); // Enable z-depth

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen


	glBindVertexArray(VAO); // Activates the vertex array object before rendering and transforming them

	CameraForwardZ = front; // Replaces camera forward vector with radians normalized as a unit vector

	// Camera movement logic
	if(currentKey == 'w' && currentKey2 == '-') // Moves down on the x axis
		cameraPosition += cameraSpeed * CameraUpX;

	if(currentKey == 's' && currentKey2 == '+') // Moves up on the x axis
		cameraPosition -= cameraSpeed * CameraUpX;

	if(currentKey == 'e')
		cameraPosition += cameraSpeed * CameraForwardZ;

	if(currentKey == 'r')
		cameraPosition -= cameraSpeed * CameraForwardZ;

	if(currentKey == 'a')
		cameraPosition -= glm::normalize(glm::cross(CameraForwardZ, CameraUpY)) * cameraSpeed;

	if(currentKey == 'd')
		cameraPosition += glm::normalize(glm::cross(CameraForwardZ, CameraUpY)) * cameraSpeed;

	// Transforms the object
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0, 0.0f, 0.0f)); // Places the object in the 0 x 0 y 0 z of the viewport.
	model = glm::rotate(model, 45.0f, glm::vec3(1.0, 1.0f, 1.0f)); // Rotates the object 12 degrees on the x axis.
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)); // Increases the object size by a scale of 2.

	// Transforms the camera
	glm::mat4 view;
	view = glm::lookAt(cameraPosition, cameraPosition + CameraForwardZ, CameraUpY);

	// Creates a perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	// Retrieves and passes transform matrices to the shader program
	GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	// Draws the triangles
	glDrawArrays(GL_TRIANGLES, 0, 78);

	glBindVertexArray(0); // Deactivate the vertex array object

	glutSwapBuffers(); // Flips the back buffer with the front buffer every frame, similar to GL Flush

}

// Creates the shader program
void UCreateShader()
{

	// Vertex shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER); // Creates the vertex shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Attaches a vertex shader to the source code.
	glCompileShader(vertexShader); // Compiles the vertex shader.

	// Fragment shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); // Creates the fragment shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL); // Attaches a fragment shader to the source code.
	glCompileShader(fragmentShader); // Compiles the fragment shader.

	//shader program
	shaderProgram = glCreateProgram(); // Creates the shader program and returns an id
	glAttachShader(shaderProgram, vertexShader); // Attach vertex shader to the shader program.
	glAttachShader(shaderProgram, fragmentShader); // Attach fragment shader to the shader program.
	glLinkProgram(shaderProgram); // Link vertex and fragment shader to the shader program.

	// Delete the vertex and fragment shaders once linked.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void UCreateBuffers()
{

	// Position and color data
	GLfloat vertices[] = {
			// Vertex Positions 					// Colors
						// Spoon back end
					   0.6875f, 0.0f, 0.0f,				0.0f, 0.0f, 0.0f,
					   0.5625f, 0.0625f, -0.0625f,		0.0f, 0.0f, 0.0f,
					   0.5625f, 0.0625f, 0.0625f,		0.0f, 0.0f, 0.0f,

					   0.6875f, 0.0f, 0.0f,				0.0f, 0.0f, 0.0f,
					   0.5625f, 0.125f, -0.0625f,		0.4f, 0.4f, 0.4f,
					   0.5625f, 0.125f, 0.0625f,		0.4f, 0.4f, 0.4f,

					   0.6875f, 0.0f, 0.0f,				0.4f, 0.4f, 0.4f,
					   0.5625f, 0.0625f, 0.0625f,		0.0f, 0.0f, 0.0f,
					   0.5625f, 0.125f, 0.0625f,		0.4f, 0.4f, 0.4f,

					   0.6875f, 0.0f, 0.0f,				0.0f, 0.0f, 0.0f,
					   0.5625f, 0.125f, -0.0625f,		0.4f, 0.4f, 0.4f,
					   0.5625f, 0.0625f, -0.0625f,		0.0f, 0.0f, 0.0f,

					   // Handle coordinates
					   0.5625f, 0.0625f, -0.0625f,		0.0f, 0.0f, 0.0f,
					   0.0f, 0.0625f, -0.0625f,			0.0f, 0.0f, 0.0f,
					   0.0f, 0.0625f, 0.0625f,			0.0f, 0.0f, 0.0f,
					   0.5625f, 0.0625f, 0.0625f,		0.0f, 0.0f, 0.0f,
					   0.0f, 0.0625f, 0.0625f,			0.0f, 0.0f, 0.0f,
					   0.5625f, 0.0625f, -0.0625f,		0.0f, 0.0f, 0.0f,

					   0.5625f, 0.0625f, 0.0625f,		0.0f, 0.0f, 0.0f,
					   0.5625f, 0.125f, 0.0625f,		0.4f, 0.4f, 0.4f,
					   0.0f, 0.0625f, 0.0625f,			0.0f, 0.0f, 0.0f,
					   0.5625f, 0.125f, 0.0625f,		0.4f, 0.4f, 0.4f,
					   0.0f, 0.125f, 0.0625f,			0.4f, 0.4f, 0.4f,
					   0.0f, 0.0625f, 0.0625f,			0.0f, 0.0f, 0.0f,


					   0.5625f, 0.0625f, -0.0625f,		0.0f, 0.0f, 0.0f,
					   0.5625f, 0.125f, -0.0625f,		0.4f, 0.4f, 0.4f,
					   0.0f, 0.125f, -0.0625f,			0.4f, 0.4f, 0.4f,
					   0.5625f, 0.0625f, -0.0625f,		0.0f, 0.0f, 0.0f,
					   0.0f, 0.125f, -0.0625f,			0.4f, 0.4f, 0.4f,
					   0.0f, 0.0625f, -0.0625f,			0.0f, 0.0f, 0.0f,

					   0.5625f, 0.125f, 0.0625f,		0.4f, 0.4f, 0.4f,
					   0.0f, 0.125f, 0.0625f,			0.4f, 0.4f, 0.4f,
					   0.0f, 0.125f, -0.0625f,			0.4f, 0.4f, 0.4f,
					   0.5625f, 0.125f, -0.0625f,		0.4f, 0.4f, 0.4f,
					   0.0f, 0.125f, -0.0625f,			0.4f, 0.4f, 0.4f,
					   0.5625f, 0.125f, 0.0625f,		0.4f, 0.4f, 0.4f,

					   -0.0625f, 0.0f, 0.03125f,		0.0f, 0.0f, 0.0f,
					   -0.0625f, 0.0f, -0.03125f,		0.0f, 0.0f, 0.0f,
					   0.0f, 0.0625f, -0.0625f,			0.0f, 0.0f, 0.0f,
					   0.0f, 0.0625f, 0.0625f,			0.0f, 0.0f, 0.0f,
					   -0.0625f, 0.0f, 0.03125f,		0.0f, 0.0f, 0.0f,
					   0.0f, 0.0625f, -0.0625f,			0.0f, 0.0f, 0.0f,

					   0.0f, 0.125f, 0.0625f,			0.4f, 0.4f, 0.4f,
					   0.0f, 0.0625f, 0.0625f,			0.0f, 0.0f, 0.0f,
					   -0.0625f, 0.0f, 0.03125f,		0.0f, 0.0f, 0.0f,
					   0.0f, 0.125f, -0.0625f,			0.4f, 0.4f, 0.4f,
					   0.0f, 0.0625f, -0.0625f,			0.0f, 0.0f, 0.0f,
					   -0.0625f, 0.0f, -0.03125f,		0.0f, 0.0f, 0.0f,

					   0.0f, 0.125f, 0.0625f,			0.4f, 0.4f, 0.4f,
					   0.0f, 0.125f, -0.0625f,			0.4f, 0.4f, 0.4f,
					   -0.0625f, 0.0f, -0.03125f,		0.0f, 0.0f, 0.0f,
					   -0.0625f, 0.0f, 0.03125f,		0.0f, 0.0f, 0.0f,
					   -0.0625f, 0.0f, -0.03125f,		0.0f, 0.0f, 0.0f,
					   0.0f, 0.125f, 0.0625f,			0.4f, 0.4f, 0.4f,

					   // Spoon head
					   -0.125f, 0.0f, 0.0f,				0.4f, 0.4f, 0.4f,
					   -0.0625f, 0.0f, -0.03125f,		0.4f, 0.4f, 0.4f,
					   -0.0625f, 0.0f, 0.03125f,		0.4f, 0.4f, 0.4f,

					   -0.125f, 0.0f, -0.125f,			0.4f, 0.4f, 0.4f,
					   -0.0625f, 0.0f, -0.03125f,		0.4f, 0.4f, 0.4f,
					   -0.125f, 0.0f, 0.0f,				0.4f, 0.4f, 0.4f,
					   -0.125f, 0.0f, 0.125f,			0.4f, 0.4f, 0.4f,
					   -0.0625f, 0.0f, 0.03125f,		0.4f, 0.4f, 0.4f,
					   -0.125f, 0.0f, 0.0f,				0.4f, 0.4f, 0.4f,

					   -0.25f, 0.0f, -0.25f,			0.4f, 0.4f, 0.4f,
					   -0.25f, 0.0f, 0.25f,				0.4f, 0.4f, 0.4f,
					   -0.125f, 0.0f, -0.125f,			0.4f, 0.4f, 0.4f,
					   -0.125f, 0.0f, 0.125f,			0.4f, 0.4f, 0.4f,
					   -0.125f, 0.0f, -0.125f,			0.4f, 0.4f, 0.4f,
					   -0.25f, 0.0f, 0.25f,				0.4f, 0.4f, 0.4f,

					   -0.3125f, 0.0f, -0.25f,			0.4f, 0.4f, 0.4f,
					   -0.3125f, 0.0f, 0.25f,			0.4f, 0.4f, 0.4f,
					   -0.25f, 0.0f, -0.25f,			0.4f, 0.4f, 0.4f,
					   -0.25f, 0.0f, 0.25f,				0.4f, 0.4f, 0.4f,
					   -0.25f, 0.0f, -0.25f,			0.4f, 0.4f, 0.4f,
					   -0.3125f, 0.0f, 0.25f,			0.4f, 0.4f, 0.4f,

					   -0.5f, .0625f, 0.0f,				0.4f, 0.4f, 0.4f,
					   -0.3125f, 0.0f, 0.25f,			0.4f, 0.4f, 0.4f,
					   -0.3125f, 0.0f, -0.25f,			0.4f, 0.4f, 0.4f

						};

	// Generate buffer ids
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Activate the vertex array object before biding and setting any VBOsand vertex attribute pointers.
	glBindVertexArray(VAO);

	// Activate the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy vertices to VBO

	// Set attribute pointer  to hold position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); // Enables vertex attributes

	// Sets attribute pointer 1 to hold color data
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6* sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // Enables vertex attribute.

	glBindVertexArray(0); // Deactivates the VAO which is good practice.
}

void UKeyboard(unsigned char key, GLint x, GLint y)
{
	switch(key){

	case 'w':
		currentKey = key;
		cout << "You pressed W!" << endl;
		break;

	case 's':
		currentKey = key;
		cout << "You pressed S!" << endl;
		break;

	case 'e':
			currentKey = key;
			cout << "You pressed E!" << endl;
			break;

	case 'r':
			currentKey = key;
			cout << "You pressed R!" << endl;
			break;

	case 'a':
		currentKey = key;
		cout << "You pressed A!" << endl;
		break;

	case 'd':
		currentKey = key;
		cout << "You pressed D!" << endl;
		break;
	case '+':
		currentKey2 = key;
		cout << "You pressed +!" << endl;
		break;
	case '-':
		currentKey2 = key;
		cout << "You pressed -!" << endl;
		break;

	default:
		cout << "Press a key!" << endl;
	}
}

// Implements the UKeyReleased function
void UKeyReleased(unsigned char key, GLint x, GLint y)
{

	cout << "Key released" << endl;
	currentKey = '0';
	currentKey2= '0';


}

void UMouseMove(int x, int y)
{
	// Immediately replaces center locked coordinates with new mouse coordinates
	if(mouseDetected)
	{
		lastMouseX = x;
		lastMouseY = y;
		mouseDetected = false;
	}
	// Gets the direction the mouse was moved in x and y
	mouseXOffset = x - lastMouseX;
	mouseYOffset = lastMouseY - y; // Inverted y

	// Updates with new mouse coordinates
	lastMouseX = x;
	lastMouseY = y;

	// Applies sensitivity to mouse direction
	mouseXOffset *= sensitivity;
	mouseYOffset *= sensitivity;

	// Accumulates the yaw and pitch variables
	yaw += mouseXOffset;
	pitch += mouseYOffset;

	// Maintains a 90 degree pitch for gimbal lock
	if(pitch > 89.0f)
		pitch = 89.0f;

	if(pitch < -89.0f)
		pitch = -89.0f;

	// Converts mouse coordinates / degrees into Radians, then to vectors
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
}
