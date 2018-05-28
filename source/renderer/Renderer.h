#pragma once

#include <vector>

#include "Mesh.h"

class Renderer
{
public:
	void draw();
	std::vector<Mesh> m_meshes;
	Mesh* create_mesh();
	void add_vertex_to_mesh(float pos_x, float pos_y, float pos_z, float normal_x, float normal_y, float normal_z, float color_r, float color_g, float color_b, Mesh* p_mesh);
};