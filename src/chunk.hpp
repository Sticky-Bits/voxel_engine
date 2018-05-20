#include <vector>
#include <future>

const int CHUNK_SIZE = 32;
const int CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
const int CHUNK_SIZE_CUBED = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

class Chunk
{
public:
	Chunk(glm::vec3 chunk_position);
    void generate_voxels();
    void generate_mesh_greedy();
	void generate_mesh_dumb();
	void load_mesh();
	void delete_buffers();
	int xyz_to_index(int x, int y, int z);
	void index_to_xyz(int index, int *x, int *y, int *z);

    std::vector<float> vertices;
    std::vector<int> indices;
    int voxels[CHUNK_SIZE_CUBED];
	glm::vec3 chunk_position;
	glm::vec3 world_position;
	unsigned int VBO, VAO, EBO;
	bool mesh_ready = false;
	std::future<void> future;

private:
};
