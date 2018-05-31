#pragma once

#include <vector>

#include "Mesh.h"
#include "../shader/Shader.h"
#include "Camera.h"
#include "../Window.h"

class Renderer
{
public:
	Renderer(Camera* p_camera, Window* p_window);
	~Renderer();
	void add_vertex_to_mesh(float pos_x, float pos_y, float pos_z, float normal_x, float normal_y, float normal_z, float color_r, float color_g, float color_b, Mesh* p_mesh);
	void set_framebuffer_size(int width, int height);
	void pre_render();
	void post_render();
	Mesh* create_mesh();

	std::vector<Mesh> m_meshes;
	Shader* mp_shader;
	Camera* mp_camera;
	Window* mp_window;
};