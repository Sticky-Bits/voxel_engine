// Hides the console on windows
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <glad/glad.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/hash.hpp> // Gives us vector hashes for maps
#include <glm/vector_relational.hpp>

#include "shaders/Shader.hpp"
#include "camera.hpp"
#include "chunk.hpp"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <queue>
#include <functional>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

void change_chunks(glm::vec3 before, glm::vec3 after, bool first_time = false);
void show_chunk(glm::vec3 chunk, bool immediate = false);
void hide_chunk(glm::vec3 chunk, bool immediate = false);
void _show_chunk(glm::vec3 chunk);
void _hide_chunk(glm::vec3 chunk);
void process_queue();
void process_entire_queue();

glm::vec3 normalize_position(glm::vec3 position);
glm::vec3 position_to_chunk(glm::vec3 position);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
static const int VIEW_DISTANCE = 0;
static const int TICKS_PER_SEC = 60;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
glm::vec3 current_chunk = position_to_chunk(camera.Position);

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Chunk map
std::unordered_map<glm::vec3, Chunk> chunks;
// Queue
std::queue<std::function<void()>> operation_queue;

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
	Shader lightingShader("shaders/vert_light.glsl", "shaders/frag_light.glsl");

	// set up chunks
	// ------------------------------------------------------------------
	change_chunks(glm::vec3(999, 0, 0), glm::vec3(0, 0, 0), false);
	
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

		// Check to see if player moved chunks
		glm::vec3 last_chunk = current_chunk;
		current_chunk = position_to_chunk(camera.Position);
		if (current_chunk != last_chunk && VIEW_DISTANCE != 0)
		{
			// Don't change for special debug case vd=0
			change_chunks(last_chunk, current_chunk);
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
		for (auto const & chunk : chunks) {
			glBindVertexArray(chunk.second.VAO);
			glm::mat4 model;
			model = glm::translate(model, chunk.second.chunk_position * (float)CHUNK_SIZE);
			lightingShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, chunk.second.vertices.size() / 9);
			// glDrawElements(GL_TRIANGLES, chunk.second.faces.size(), GL_INT, 0);
		}

		float start = glfwGetTime();
		while (!operation_queue.empty() && glfwGetTime() - start < 1.0 / TICKS_PER_SEC)
		{
			process_queue();
		}

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
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
		std::cout << "Number of loaded chunks: " << chunks.size() << std::endl;
	}
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

template<typename _Callable, typename... _Args>
void queue_function(_Callable&& __f, _Args&&... __args)
{
	std::function<void()> func = std::bind(std::forward<_Callable>(__f), std::forward<_Args>(__args)...);
	operation_queue.push(func);
}

void process_queue()
{
	std::function<void()> func = operation_queue.front();
	operation_queue.pop();
	func();
}

void process_entire_queue()
{
	while(!operation_queue.empty())
		process_queue();
}

struct compareVec
{
    bool operator() (const glm::vec3& lhs, const glm::vec3& rhs) const
    {
		if(lhs.x < rhs.x)
		return true;
		else if(lhs.x > rhs.x)
		return false;
		else
		{
			if(lhs.y < rhs.y)
			return true;
			else if(lhs.y > rhs.y)
			return false;
			else
			{
				if(lhs.z < rhs.z)
				return true;
				else if(lhs.z > rhs.z)
				return false;
				else
				{
					return false;
				}
			}
		}
    }
};

void change_chunks(glm::vec3 before, glm::vec3 after, bool first_time/* = false*/)
{
	std::set<glm::vec3, compareVec > before_set, after_set;
	if(VIEW_DISTANCE == 0)
	{
		// Special debug case
		before_set.insert(glm::vec3(before.x, before.y, before.z));
		after_set.insert(glm::vec3(0, 0, 0));
	} else {
		for (int dx = -VIEW_DISTANCE; dx < VIEW_DISTANCE + 1; dx++)
		{
			for (int dy = 0; dy < 1; dy++)//(int dy = -VIEW_DISTANCE; dy < VIEW_DISTANCE + 1; dy++)
			{
				for (int dz = -VIEW_DISTANCE; dz < VIEW_DISTANCE + 1; dz++)
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
	}

	std::vector<glm::vec3> show_set, hide_set;

	std::set_difference(after_set.begin(), after_set.end(), before_set.begin(), before_set.end(), std::inserter(show_set, show_set.begin()), compareVec());
	std::set_difference(before_set.begin(), before_set.end(), after_set.begin(), after_set.end(), std::inserter(hide_set, hide_set.end()), compareVec());

	for (auto chunk : show_set)
	{
		// Show chunks, do immediate mode if first time
		show_chunk(chunk, first_time);
	}
	// Dont hide any chunks if first time
	if(!first_time)
	{
		for (auto chunk : hide_set)
		{
			hide_chunk(chunk);
		}
	}
}

void show_chunk(glm::vec3 chunk, bool immediate)
{
	if(immediate)
		_show_chunk(chunk);
	else
		queue_function(_show_chunk, chunk);
}

void _show_chunk(glm::vec3 chunk)
{
	//Make pair of vec3:Chunk called insert_chunk of (chunk_position, newchunkat(position))
	chunks.emplace(chunk, Chunk(chunk));
}

void hide_chunk(glm::vec3 chunk, bool immediate)
{
	if(immediate)
		_hide_chunk(chunk);
	else
		queue_function(_hide_chunk, chunk);
}

void _hide_chunk(glm::vec3 chunk)
{
	chunks.erase(chunk);
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
	return glm::vec3(floor(position.x / CHUNK_SIZE), /*floor(position.y / CHUNK_SIZE)*/0, floor(position.z / CHUNK_SIZE));
}