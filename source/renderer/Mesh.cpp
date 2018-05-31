#include "Mesh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	// Delete the vertices
	// https://stackoverflow.com/a/10465032
	std::vector<float>().swap(m_vertices);
}

void Mesh::generate_buffers()
{
	// Only bother if any vertices exist
	if (m_vertices.size() > 0)
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);

		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

		// Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// Normals
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// Color
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	// Delete buffers?
}