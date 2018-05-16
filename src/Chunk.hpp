#include <glad/glad.hpp>
#include <vector>
#include <array>

const int CHUNK_SIZE = 16;
const int CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
const int CHUNK_SIZE_CUBED = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

class Chunk
{
public:
	Chunk(glm::vec3 chunk_position);
    void generate_voxels();
    void generate_mesh_greedy_old();
    void generate_mesh_greedy();
	void generate_mesh_dumb();
	void load_mesh();
	void delete_buffers();
	int xyz_to_index(int x, int y, int z);
	void index_to_xyz(int index, int *x, int *y, int *z);

    std::vector<float> vertices;
    std::vector<int> faces;
    int voxels[CHUNK_SIZE_CUBED];
	glm::vec3 chunk_position;
	glm::vec3 world_position;
	unsigned int VBO, VAO, EBO;

private:
};

Chunk::Chunk(glm::vec3 position)
{
	chunk_position = position;
	world_position = chunk_position * (float)CHUNK_SIZE;
    generate_voxels();
    generate_mesh_dumb();
	load_mesh();
}

void Chunk::generate_voxels()
{
    // Generate Voxels
	// TODO: Change from bool to int? for different types of voxels
	// Then generate meshes seperately for different types
	// And push a color/texture to shader per type
	int i = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
	for (int y = 0; y < CHUNK_SIZE; y++)
	for (int z = 0; z < CHUNK_SIZE; z++, i++)
    {
		if (y<2) {
			voxels[i] = 1;
		} else {
			voxels[i] = 0;
		}
	}
}

int Chunk::xyz_to_index(int x, int y, int z){
	return (z * CHUNK_SIZE_SQUARED) + (y * CHUNK_SIZE) + (z * CHUNK_SIZE);
}

void Chunk::index_to_xyz(int index, int *x, int *y, int *z){
	*x = index % CHUNK_SIZE;
	*y = (index / CHUNK_SIZE) % CHUNK_SIZE;
	*z = index / CHUNK_SIZE_SQUARED;
}

void Chunk::generate_mesh_greedy()
{
	for (int d=0; d<3; d++){
		int i, j, k, l, w, h
        , u = (d+1)%3
        , v = (d+2)%3
        , x[] = {0,0,0}
        , q[] = {0,0,0};
        int mask[CHUNK_SIZE_SQUARED] = {0};
        q[d] = 1;
        for (x[d] = -1; x[d] < CHUNK_SIZE; )
        {
            // Compute mask. mask contains ints referring to voxel positions to render
            int n = 0;
            for (x[v] = 0; x[v] < CHUNK_SIZE; ++x[v])
            {
                for (x[u] = 0; x[u] < CHUNK_SIZE; ++x[u], ++n)
                {
					int a = (0    <= x[d]          ? xyz_to_index(x[0], x[1], x[2])                : 0);
                    int b = (x[d] < CHUNK_SIZE - 1 ? xyz_to_index(x[0]+q[0], x[1]+q[1], x[2]+q[2]) : 0);
					if((!!a) == (!!b)) { // If c++ breaks on this, it's just int->bool casting
						mask[n] = 0;
					} else if(!!a) {
						mask[n] = a;
					} else {
						mask[n] = -b;
					}
                }
            }
            x[d]++;
            // Generate mesh for mask using lexicographic ordering
            n = 0;
            for (j = 0; j < CHUNK_SIZE; ++j)
            {
                for (i = 0; i < CHUNK_SIZE; )
                {
					int c = mask[n];
                    if(!!c)
                    {
                        //Compute width
                        for (w = 1; c == mask[n+w] && i+w < CHUNK_SIZE; ++w)
                        {}
                        //Compute height
                        bool done = false;
                        for (h = 1; j+h < CHUNK_SIZE; ++h)
                        {
                            for (k = 0; k < w; ++k)
                            {
                                if (c != mask[n+k+h*CHUNK_SIZE])
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
						// Add vertices
						x[u] = i;
						x[v] = j;
						int du[] = {0,0,0};
						int dv[] = {0,0,0};
						if(c > 0){
							dv[v] = h;
							du[u] = w;
						} else {
							c = -c;
							du[v] = h;
							dv[u] = w;
						}
						int vertex_count = vertices.size();
						vertices.push_back(x[0]                );//x1
						vertices.push_back(x[1]                );//y1
						vertices.push_back(x[2]                );//z1
						vertices.push_back(x[0] + du[0]        );//x2
						vertices.push_back(x[1] + du[1]        );//y2
						vertices.push_back(x[2] + du[2]        );//z2
						vertices.push_back(x[0] + du[0] + dv[0]);//x3
						vertices.push_back(x[1] + du[1] + dv[1]);//y3
						vertices.push_back(x[2] + du[2] + dv[2]);//z3
						vertices.push_back(x[0]                );//x1
						vertices.push_back(x[1]                );//y1
						vertices.push_back(x[2]                );//z1
						vertices.push_back(x[0] + du[0] + dv[0]);//x3
						vertices.push_back(x[1] + du[1] + dv[1]);//y3
						vertices.push_back(x[2] + du[2] + dv[2]);//z3
						vertices.push_back(x[0]         + dv[0]);//x4
						vertices.push_back(x[1]         + dv[1]);//y4
						vertices.push_back(x[2]         + dv[2]);//z4

						// // I think faces are indices
						// faces.push_back(vertex_count);
						// faces.push_back(vertex_count+1);
						// faces.push_back(vertex_count+2);
						// // faces.push_back(c); // Might be color?
						// faces.push_back(vertex_count);
						// faces.push_back(vertex_count+2);
						// faces.push_back(vertex_count+3);
						// // faces.push_back(c); // Might be color?
					}
				}
			}
		}
	}
}

void Chunk::generate_mesh_greedy_old()
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
        bool mask[CHUNK_SIZE_SQUARED] = {false};
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
                        (0    <= x[d]          ? voxels[xyz_to_index(x[0], x[1], x[2])]                : false) !=
                        (x[d] < CHUNK_SIZE - 1 ? voxels[xyz_to_index(x[0]+q[0], x[1]+q[1], x[2]+q[2])] : false);
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
						vertices.push_back(x[0]);//texture x
						vertices.push_back(x[1]);//texture y

						vertices.push_back(x[0] + du[0]);
						vertices.push_back(x[1] + du[1]);
						vertices.push_back(x[2] + du[2]);
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(x[0] + du[0]);//texture x
						vertices.push_back(x[1] + du[1]);//texture y

						vertices.push_back(x[0] + du[0] + dv[0]);
						vertices.push_back(x[1] + du[1] + dv[1]);
						vertices.push_back(x[2] + du[2] + dv[2]);
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(x[0] + du[0] + dv[0]);//texture x
						vertices.push_back(x[1] + du[1] + dv[1]);//texture y

						vertices.push_back(x[0] + du[0] + dv[0]);
						vertices.push_back(x[1] + du[1] + dv[1]);
						vertices.push_back(x[2] + du[2] + dv[2]);
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(x[0] + du[0] + dv[0]);//texture x
						vertices.push_back(x[1] + du[1] + dv[1]);//texture y

						vertices.push_back(x[0] + dv[0]);
						vertices.push_back(x[1] + dv[1]);
						vertices.push_back(x[2] + dv[2]);
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(x[0] + dv[0]);//texture x
						vertices.push_back(x[1] + dv[1]);//texture y

						vertices.push_back(x[0]);//x
						vertices.push_back(x[1]);//y
						vertices.push_back(x[2]);//z
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(x[0]);//texture x
						vertices.push_back(x[1]);//texture y
						// TODO: Check correct winding order for face culling
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
}

void Chunk::generate_mesh_dumb(){
	// Define a dumb mesh generator. Might be useful for testing
	int x[] = {0,0,0};
	for (x[2]=0; x[2]<CHUNK_SIZE; x[2]++){
		for (x[1]=0; x[1]<CHUNK_SIZE; x[1]++){
			for (x[0]=0; x[0]<CHUNK_SIZE; x[0]++){
				// if (voxels[xyz_to_index(x[2],x[1],x[0])]){
					for (int d=0; d<3; d++){
						int t[] = {x[0],x[1],x[2]};
						int u[] = {0,0,0};
						int v[] = {0,0,0};
						u[(d+1)%3] = 1;
						v[(d+2)%3] = 1;
						for (int s=0; s<2; s++){
							t[d] = x[d] + s;
							vertices.push_back(t[0]);
							vertices.push_back(t[1]);
							vertices.push_back(t[2]);

							vertices.push_back(t[0]+u[0]);
							vertices.push_back(t[1]+u[1]);
							vertices.push_back(t[2]+u[2]);

							vertices.push_back(t[0]+u[0]+v[0]);
							vertices.push_back(t[1]+u[1]+v[1]);
							vertices.push_back(t[2]+u[2]+v[2]);

							vertices.push_back(t[0]);
							vertices.push_back(t[1]);
							vertices.push_back(t[2]);

							vertices.push_back(t[0]+v[0]);
							vertices.push_back(t[1]+v[1]);
							vertices.push_back(t[2]+v[2]);

							vertices.push_back(t[0]+u[0]+v[0]);
							vertices.push_back(t[1]+u[1]+v[1]);
							vertices.push_back(t[2]+u[2]+v[2]);
						}
					}
				// }
			}
		}
	}
}

void Chunk::load_mesh(){
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(int), &faces[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// glEnableVertexAttribArray(2);
}

void Chunk::delete_buffers()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}