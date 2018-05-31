#include "Renderer.h"

#include "../Game.h"

// #include <glad/glad.h>
#include "../shader/Shader.h"

Renderer::Renderer(Camera* p_camera, Window* p_window)
{
	mp_shader = new Shader("shaders/vert_light.glsl", "shaders/frag_light.glsl");
	mp_camera = p_camera;
	mp_window = p_window;

	mp_shader->use();
	mp_shader->setInt("material.diffuse", 0);
	mp_shader->setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

	glEnable(GL_DEPTH_TEST);
}

Renderer::~Renderer()
{
	// TODO: Delete shader here? mm is hard yo
}

Mesh* Renderer::create_mesh()
{
	Mesh* p_new_mesh = new Mesh();
	return p_new_mesh;
}

void Renderer::add_vertex_to_mesh(float pos_x, float pos_y, float pos_z, float normal_x, float normal_y, float normal_z, float color_r, float color_g, float color_b, Mesh* p_mesh)
{
	p_mesh->m_vertices.push_back(pos_x);
	p_mesh->m_vertices.push_back(pos_y);
	p_mesh->m_vertices.push_back(pos_z);
	p_mesh->m_vertices.push_back(normal_x);
	p_mesh->m_vertices.push_back(normal_y);
	p_mesh->m_vertices.push_back(normal_z);
	p_mesh->m_vertices.push_back(color_r);
	p_mesh->m_vertices.push_back(color_g);
	p_mesh->m_vertices.push_back(color_b);
}

void Renderer::set_framebuffer_size(int width, int height)
{
	glViewport(0, 0, width, height);
}

void Renderer::pre_render()
{
	// Seems wrong that the renderer doesn't know about the camera or the window...
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mp_shader->use();
	mp_shader->setVec3("light.direction", -0.5f, -1.0f, -0.0f);
	mp_shader->setVec3("viewPos", mp_camera->Position);

	// light properties
	mp_shader->setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
	mp_shader->setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
	mp_shader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	mp_shader->setFloat("material.shininess", 32.0f);

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(mp_camera->Zoom), (float)mp_window->m_window_width / (float)mp_window->m_window_height, 0.1f, 1000.0f);
	glm::mat4 view = mp_camera->GetViewMatrix();
	mp_shader->setMat4("projection", projection);
	mp_shader->setMat4("view", view);
}

void Renderer::post_render()
{
	mp_window->swap_buffers();
	glfwPollEvents();
}
