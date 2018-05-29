#pragma once

#include <vector>

#include "../renderer/Renderer.h"

class ChunkManager;
class Settings;

class Chunk
{
public:
	Chunk(Renderer * p_renderer, ChunkManager * p_chunk_manager, Settings* p_settings);
	void generate_voxels();
	void generate_mesh_greedy();
	void load_mesh();
	void delete_buffers();
	int xyz_to_index(int x, int y, int z);
	void index_to_xyz(int index, int *x, int *y, int *z);

	unsigned int VBO, VAO;

	// A new world order
	void set_position(glm::vec3 position);
	void setup();

	Mesh* mp_mesh;
	Renderer* mp_renderer;
	ChunkManager* mp_chunk_manager;
	Settings* mp_settings;
	glm::vec3 m_position;

	static const int CHUNK_SIZE = 32;
	static const int CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
	static const int CHUNK_SIZE_CUBED = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

	int voxels[CHUNK_SIZE_CUBED];

private:
};
