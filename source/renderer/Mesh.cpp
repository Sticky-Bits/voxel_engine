#include "Mesh.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	// Delete the vertices
	// https://stackoverflow.com/a/10465032
	std::vector<float>().swap(m_vertices);
}