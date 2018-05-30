#pragma once

#include <vector>

class Mesh
{
public:
	Mesh();
	~Mesh();
	void generate_buffers();
	std::vector<float> m_vertices;
	unsigned int m_vao, m_vbo;
};