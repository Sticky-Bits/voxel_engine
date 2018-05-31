// Hides the console on windows
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar/AntTweakBar.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp> // Gives us vector hashes for maps
#include <glm/vector_relational.hpp>

#include "shader/Shader.h"
#include "renderer/Camera.h"
#include "blocks/Chunk.h"
#include "blocks/ChunkManager.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <queue>
#include <functional>
#include <iterator>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

glm::vec3 normalize_position(glm::vec3 position);
glm::vec3 position_to_chunk(glm::vec3 position);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
static const int TICKS_PER_SEC = 60;

double somestupidvar = 0.0f;

// camera
glm::vec3 starting_position = glm::vec3(0.0f, 0.0f, 3.0f);
Camera camera(starting_position);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
glm::vec3 current_chunk = position_to_chunk(camera.Position);
bool mouse_captured = false;

// timing
double deltaTime = 0.0f;
double lastFrame = 0.0f;


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel Engine", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader lightingShader("shaders/vert_light.glsl", "shaders/frag_light.glsl");

	// Draw in wireframe
	glPointSize(5.0f);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	//unsigned int diffuseMap = loadTexture("textures/blocks/grass_side.png");
	//unsigned int specularMap = loadTexture("textures/blocks/grass_top.png");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

	ChunkManager chunk_manager(glm::vec3(0,0,0));

	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(SCR_WIDTH, SCR_HEIGHT);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// tell GLFW to capture our mouse
		if (mouse_captured)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Check to see if player moved chunks
		// TODO: Move this into ChunkManager
		glm::vec3 last_chunk = current_chunk;
		current_chunk = position_to_chunk(camera.Position);
		if (current_chunk != last_chunk)
		{
			// Don't change for special debug case vd=0
			chunk_manager.change_chunks(last_chunk, current_chunk);
		}

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		//lightingShader.setVec3("light.direction", -0.0f, -1.0f, sin(glfwGetTime()));
		lightingShader.setVec3("light.direction", -0.5f, -1.0f, -0.0f);
		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		lightingShader.setFloat("material.shininess", 32.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// Bind textures
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, diffuseMap);

		// Draw Chunks
		for (auto chunk : chunk_manager.chunks) {
			glBindVertexArray(chunk.second.VAO);
			glm::mat4 model;
			model = glm::translate(model, chunk.second.chunk_position * (float)CHUNK_SIZE);
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, chunk.second.vertices.size() / 9);
			// glDrawElements(GL_TRIANGLES, chunk.second.faces.size(), GL_INT, 0);
		}

		double start = glfwGetTime();
		while (!chunk_manager.operation_queue.empty() && glfwGetTime() - start < 1.0 / TICKS_PER_SEC)
		{
			chunk_manager.process_chunk_queue();
		}

		TwDraw();

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

	// clean up chunks
	/*for (std::vector<Chunk>::iterator it = chunks.begin(); it != chunks.end(); ++it) {
	Chunk& chunk = *it;
	chunk.delete_buffers();
	}*/

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	TwTerminate();
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (!TwEventKeyGLFW(key, action))
	{

	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (!TwEventMousePosGLFW(xpos, ypos))
	{
		if (mouse_captured)
			camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (!TwEventMouseWheelGLFW(yoffset))
	{
		if (mouse_captured)
			camera.ProcessMouseScroll(yoffset);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (!TwEventMouseButtonGLFW(button, action))
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			mouse_captured = true;
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
			mouse_captured = false;
	}

}

glm::vec3 normalize_position(glm::vec3 position)
{
	// Accepts a position of any precision and returns the block containing that position
	return glm::vec3(floor(position.x), floor(position.y), floor(position.z));
}

glm::vec3 position_to_chunk(glm::vec3 position)
{
	// Accepts a position of any precision and returns the chunk coordinates containing that position
	glm::vec3 normalized_position = normalize_position(position);
	return glm::vec3(floor(position.x / CHUNK_SIZE), 0/*floor(position.y / CHUNK_SIZE)*/, floor(position.z / CHUNK_SIZE));
}
