#include "Renderer.h"
#include "../shader/Shader.h"

Renderer::Renderer()
{
	mp_shader = new Shader("shaders/vert_light.glsl", "shaders/frag_light.glsl");
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
