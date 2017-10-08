#include <glad/glad.h>
#include <vector>
#include <array>

const int CHUNK_SIZE = 16;

class Chunk
{
public:
	Chunk(glm::vec3 chunk_position);
    void gen_voxels();
    void gen_vertices();
	void delete_buffers();

    std::vector<float> vertices;
    std::array<std::array<std::array<int, CHUNK_SIZE>, CHUNK_SIZE>, CHUNK_SIZE> voxels;
	glm::vec3 position;
	unsigned int VBO, VAO;

private:
};

Chunk::Chunk(glm::vec3 chunk_position)
{
	position = chunk_position;
    gen_voxels();
    gen_vertices();
}

void Chunk::gen_voxels()
{
    // set all voxels to true for testing
    for (int x = 0; x < CHUNK_SIZE; x++)
    {
        for (int y = 0; y < CHUNK_SIZE; y++)
        {
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if (y < 3+(rand()%2))
				{
					voxels[x][y][z] = true;
				}
				else
				{
					voxels[x][y][z] = false;
				}
			}
        }
    }
}

void Chunk::gen_vertices()
{
    // Sweep over 3 axes
    for (int d = 0; d < 3; d++)
    {
        int i, j, k, l, w, h
        , u = (d+1)%3
        , v = (d+2)%3
        , x[] = {0,0,0}
        , q[] = {0,0,0};
		// Crude Normals
		float normalx, normaly, normalz;
		// d0 = x, d1 = y, d2 = z
		if (d == 0)
		{
			normalx = 1.0f;
			normaly = 0.0f;
			normalz = 0.0f;
		}
		else if (d == 1)
		{
			normalx = 0.0f;
			normaly = 1.0f;
			normalz = 0.0f;
		}
		else // d == 2
		{
			normalx = 0.0f;
			normaly = 0.0f;
			normalz = 1.0f;
		}
        bool mask[CHUNK_SIZE * CHUNK_SIZE] = {false};
        q[d] = 1;
        for (x[d] = -1; x[d] < CHUNK_SIZE; )
        {
            // Compute mask
            int n = 0;
            for (x[v] = 0; x[v] < CHUNK_SIZE; ++x[v])
            {
                for (x[u] = 0; x[u] < CHUNK_SIZE; ++x[u])
                {
                    mask[n++] = 
                        (0    <= x[d]          ? voxels[x[0]][x[1]][x[2]]                : false) !=
                        (x[d] < CHUNK_SIZE - 1 ? voxels[x[0]+q[0]][x[1]+q[1]][x[2]+q[2]] : false);
                }
            }
            ++x[d];
            // Generate mesh for mask using lexicographic ordering
            n = 0;
            for (j = 0; j < CHUNK_SIZE; ++j)
            {
                for (i = 0; i < CHUNK_SIZE; )
                {
                    if(mask[n])
                    {
                        //Compute width
                        for (w = 1; mask[n+w] && i+w < CHUNK_SIZE; ++w)
                        {}
                        //Compute height
                        bool done = false;
                        for (h = 1; j+h < CHUNK_SIZE; ++h)
                        {
                            for (k = 0; k < w; ++k)
                            {
                                if (!mask[n+k+h*CHUNK_SIZE])
                                {
                                    done = true;
                                    break;
                                }
                            }
                            if (done)
                            {
                                break;
                            }
                        }
                        // Add vertex data
						x[u] = i; x[v] = j;
						int du[] = { 0,0,0 }; du[u] = w;
						int dv[] = { 0,0,0 }; dv[v] = h;
						vertices.push_back(x[0]);//x
						vertices.push_back(x[1]);//y
						vertices.push_back(x[2]);//z
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);

						vertices.push_back(x[0] + du[0]);
						vertices.push_back(x[1] + du[1]);
						vertices.push_back(x[2] + du[2]);
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);

						vertices.push_back(x[0] + du[0] + dv[0]);
						vertices.push_back(x[1] + du[1] + dv[1]);
						vertices.push_back(x[2] + du[2] + dv[2]);
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);

						vertices.push_back(x[0] + du[0] + dv[0]);
						vertices.push_back(x[1] + du[1] + dv[1]);
						vertices.push_back(x[2] + du[2] + dv[2]);
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);

						vertices.push_back(x[0] + dv[0]);
						vertices.push_back(x[1] + dv[1]);
						vertices.push_back(x[2] + dv[2]);
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);

						vertices.push_back(x[0]);//x
						vertices.push_back(x[1]);//y
						vertices.push_back(x[2]);//z
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						// Correct winding order for culling?
                        // Zero out mask
                        for (l=0; l<h; ++l)
                        {
                            for (k=0; k<w; ++k)
                            {
                                mask[n+k+l*CHUNK_SIZE] = false;
                            }
                        }
                        //Increment counters and continue
                        i += w; n += w;
                    } else {
                        ++i; ++n;
                    }
                }
            }
        }
    }
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
}

void Chunk::delete_buffers()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}