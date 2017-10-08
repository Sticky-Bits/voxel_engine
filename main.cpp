// Hides the console on windows
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp> // Gives us vector hashes for maps
#include <glm/vector_relational.hpp>

#include "shaders/Shader.h"
#include "Camera.h"
#include "Chunk.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

void change_chunks(glm::vec3 before, glm::vec3 after);
void show_chunk(glm::vec3 chunk);
void hide_chunk(glm::vec3 chunk);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
static const int VIEW_DISTANCE = 3;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
	Shader lightingShader("shaders/vertex.glsl", "shaders/fragment.glsl");
	Shader lampShader("shaders/light_v.glsl", "shaders/light_f.glsl");

	// set up chunks
	// ------------------------------------------------------------------
	//Chunk chunk(glm::vec3(-1,0,0));
	/*std::vector<Chunk> chunks = {
		Chunk(glm::vec3(0, 0, 0)),
		Chunk(glm::vec3(-1,0,0)),
		Chunk(glm::vec3(1,0,0)),
		Chunk(glm::vec3(1, 0, 1)),
		Chunk(glm::vec3(-1, 0, 1)),
		Chunk(glm::vec3(1, 0, -1)),
		Chunk(glm::vec3(-1, 0, -1)),
		Chunk(glm::vec3(0, 0, -1)),
		Chunk(glm::vec3(0, 0, 1)),
	};*/
	std::unordered_map<glm::vec3, Chunk> chunks;
	
	// Draw in wireframe
	glPointSize(5.0f);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	//unsigned int diffuseMap = loadTexture("textures/blocks/grass_side.png");
	//unsigned int specularMap = loadTexture("textures/blocks/grass_top.png");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuse", 0);
	lightingShader.setInt("material.specular", 1);
	lightingShader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));


	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("light.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("viewPos", camera.Position);

		// light properties
		lightingShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		lightingShader.setFloat("material.shininess", 32.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// Draw Chunks
		//for (std::vector<Chunk>::iterator it = chunks.begin(); it != chunks.end(); ++it) {
		//	Chunk& chunk = *it;
		//	glBindVertexArray(chunk.VAO);
		//	glm::mat4 model;
		//	model = glm::translate(model, chunk.position * (float)CHUNK_SIZE);
		//	lightingShader.setMat4("model", model);
		//	glDrawArrays(GL_TRIANGLES, 0, chunk.vertices.size() / 6);
		//	/* std::cout << *it; ... */
		//}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------

	// for chunk in chunks:
	/*for (std::vector<Chunk>::iterator it = chunks.begin(); it != chunks.end(); ++it) {
		Chunk& chunk = *it;
		chunk.delete_buffers();
	}*/

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

bool myfunc(glm::vec3 a, glm::vec3 b)
{
	return true;
}

struct compareVec
{
	bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
	{
		return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
	}
};

void change_chunks(glm::vec3 before, glm::vec3 after)
{
	std::set<glm::vec3, compareVec > before_set, after_set;
	for (int dx = -VIEW_DISTANCE; dx < VIEW_DISTANCE; dx++)
	{
		for (int dy = 0; dy < 3; dy++)//(int dy = -VIEW_DISTANCE; dy < VIEW_DISTANCE; dy++)
		{
			for (int dz = -VIEW_DISTANCE; dz < VIEW_DISTANCE; dz++)
			{
				if (pow(dx, 2) + pow(dy, 2) + pow(dz, 2) > pow((VIEW_DISTANCE + 1), 2))
				{
					continue;
				}
				before_set.insert(glm::vec3(before.x + dx, before.y + dy, before.z + dz));
				after_set.insert(glm::vec3(after.x + dx, after.y + dy, after.z + dz));
			}
		}
	}

	std::set<glm::vec3> show_set, hide_set;
	std::set_difference(after_set.begin(), after_set.end(), before_set.begin(), before_set.end(), std::inserter(show_set, show_set.begin()));
	//std::set_difference(before_set.begin(), before_set.end(), after_set.begin(), after_set.end(), std::inserter(hide_set, hide_set.end()));

	/*for (auto chunk : show_set)
	{
		std::cout << "Showing: " << chunk.x << ", " << chunk.y << ", " << chunk.z << std::endl;
		show_chunk(chunk);
	}
	for (auto chunk : hide_set)
	{
		std::cout << "Hiding: " << chunk.x << ", " << chunk.y << ", " << chunk.z << std::endl;
		hide_chunk(chunk);
	}*/
	
}

void show_chunk(glm::vec3 chunk)
{
}
void hide_chunk(glm::vec3 chunk)
{
}