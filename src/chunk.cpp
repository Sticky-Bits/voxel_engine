#include <glad/glad.hpp>
#include <vector>
#include <array>
#include <glm/glm.hpp>
#include <iostream>

#include "chunk.hpp"

Chunk::Chunk(glm::vec3 position)
{
	chunk_position = position;
	world_position = chunk_position * (float)CHUNK_SIZE;
    generate_voxels();
	generate_mesh_greedy();
	load_mesh();
}

void Chunk::generate_voxels()
{
    // Generate Voxels
	// TODO: Change from bool to int? for different types of voxels
	// Then generate meshes seperately for different types
	// And push a color/texture to shader per type
	const float PI = 3.141592653;
	int i = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
	for (int y = 0; y < CHUNK_SIZE; y++)
	for (int z = 0; z < CHUNK_SIZE; z++, i++)
    {
		// if (y < CHUNK_SIZE/2)
		// 	voxels[i] = 1;
		// else
		// 	voxels[i] = 2;

		// Hills
		float h0 = 3.0 * sin(PI * z / 12.0 - PI * x * 0.1) + 27;
		if(y > h0+1) {
			voxels[i] = 0;
			continue;
		}
		if(h0 <= y) {
			voxels[i] = 1;
			continue;
		}
		float h1 = 2.0 * sin(PI * z * 0.25 - PI * x * 0.3) + 20;
		if(h1 <= y) {
			voxels[i] = 2;
			continue;
		}
		if(2 < y) {
			voxels[i] = random() < 0.1 ? 3 : 4;
			continue;
		}
		voxels[i] = 5;

		// Valley
		//y <= (z*z + x*x) * 31 / (32*32*2) + 1 ? voxels[i] = 1 : voxels[i] = 0;
	}
}

int Chunk::xyz_to_index(int x, int y, int z){
	return z + CHUNK_SIZE * (y + CHUNK_SIZE * x);
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

		// Set normals for this sweep
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

        for (x[d] = -1; x[d] < CHUNK_SIZE; )
        {
            // Compute mask
            int n = 0;
            for (x[v] = 0; x[v] < CHUNK_SIZE; ++x[v])
            {
                for (x[u] = 0; x[u] < CHUNK_SIZE; ++x[u], ++n)
                {
					int a = (0    <= x[d]          ? voxels[xyz_to_index(x[0], x[1], x[2])]                : 0);
                    int b = (x[d] < CHUNK_SIZE - 1 ? voxels[xyz_to_index(x[0]+q[0], x[1]+q[1], x[2]+q[2])] : 0);
					if(((bool)a) == ((bool)b)) {
						mask[n] = 0;
					} else if((bool)a) {
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
					int voxel_type = mask[n];
					// Only generate vertices for non-zero voxel types
                    if((bool)voxel_type)
                    {
                        //Compute width
                        for (w = 1; voxel_type == mask[n+w] && i+w < CHUNK_SIZE; ++w)
                        {}
                        //Compute height
                        bool done = false;
                        for (h = 1; j+h < CHUNK_SIZE; ++h)
                        {
                            for (k = 0; k < w; ++k)
                            {
                                if (voxel_type != mask[n+k+h*CHUNK_SIZE])
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
						if(voxel_type > 0){
							dv[v] = h;
							du[u] = w;
						} else {
							voxel_type = -voxel_type;
							du[v] = h;
							dv[u] = w;
						}
						// Get current vertex size for adding indicies later
						int vertex_count = vertices.size();

						// Set colors
						float color_r, color_g, color_b;
						switch (voxel_type) {
							// Grass
							case 1: color_r = 0.10f; color_g = 0.86f; color_b = 0.19f; break;
							// Dirt
							case 2: color_r = 0.58f; color_g = 0.29f; color_b = 0.00f; break;
							// Stone
							case 3: color_r = 0.34f; color_g = 0.34f; color_b = 0.34f; break;
							// Dark Stone / Coal
							case 4: color_r = 0.66f; color_g = 0.66f; color_b = 0.66f; break;
							// lava? / red
							case 5: color_r = 0.50f; color_g = 0.00f; color_b = 0.00f; break;
						}
						// TODO: Check correct winding order for face culling
						vertices.push_back(x[0]                );//x0
						vertices.push_back(x[1]                );//y0
						vertices.push_back(x[2]                );//z0
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(color_r);
						vertices.push_back(color_g);
						vertices.push_back(color_b);
						vertices.push_back(x[0] + du[0]        );//x1
						vertices.push_back(x[1] + du[1]        );//y1
						vertices.push_back(x[2] + du[2]        );//z1
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(color_r);
						vertices.push_back(color_g);
						vertices.push_back(color_b);
						vertices.push_back(x[0] + du[0] + dv[0]);//x2
						vertices.push_back(x[1] + du[1] + dv[1]);//y2
						vertices.push_back(x[2] + du[2] + dv[2]);//z2
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(color_r);
						vertices.push_back(color_g);
						vertices.push_back(color_b);
						vertices.push_back(x[0]                );//x0
						vertices.push_back(x[1]                );//y0
						vertices.push_back(x[2]                );//z0
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(color_r);
						vertices.push_back(color_g);
						vertices.push_back(color_b);
						vertices.push_back(x[0] + du[0] + dv[0]);//x2
						vertices.push_back(x[1] + du[1] + dv[1]);//y2
						vertices.push_back(x[2] + du[2] + dv[2]);//z2
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(color_r);
						vertices.push_back(color_g);
						vertices.push_back(color_b);
						vertices.push_back(x[0]         + dv[0]);//x3
						vertices.push_back(x[1]         + dv[1]);//y3
						vertices.push_back(x[2]         + dv[2]);//z3
						vertices.push_back(normalx);
						vertices.push_back(normaly);
						vertices.push_back(normalz);
						vertices.push_back(color_r);
						vertices.push_back(color_g);
						vertices.push_back(color_b);

						// TODO: Move to using indexed drawing + EBOs
						// push indicies. There will be some overlap here, possible optimization
						// indices.push_back(vertex_count);
						// indices.push_back(vertex_count+1);
						// indices.push_back(vertex_count+2);
						// indices.push_back(vertex_count);
						// indices.push_back(vertex_count+2);
						// indices.push_back(vertex_count+3);

						// TODO: use voxel_type to generate colors or something

						// TODO: generate normals

                        // Zero out mask
                        for (l=0; l<h; ++l)
                        {
                            for (k=0; k<w; ++k)
                            {
                                mask[n+k+l*CHUNK_SIZE] = 0;
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
	// Doesn't actually work atm :(
	int x[] = {0,0,0}; int n = 0;
	for (x[2]=0; x[2]<CHUNK_SIZE; x[2]++){
		for (x[1]=0; x[1]<CHUNK_SIZE; x[1]++){
			for (x[0]=0; x[0]<CHUNK_SIZE; x[0]++, n++){
				if (voxels[n]){
					for (int d=0; d<3; d++){
						int t[] = {x[0],x[1],x[2]};
						int u[] = {0,0,0};
						int v[] = {0,0,0};
						u[(d+1)%3] = 1;
						v[(d+2)%3] = 1;
						for (int s=0; s<2; s++){
							t[d] = x[d] + s;
							// There must be a better way to do this...
							int tmp[] = {0,0,0};
							tmp[0] = u[0]; tmp[1] = u[1]; tmp[2] = u[2];
							u[0] = v[0]; u[1] = v[1]; u[2] = v[2];
							v[0] = tmp[0]; v[1] = tmp[1]; v[2] = tmp[2];
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

							vertices.push_back(t[0]+u[0]+v[0]);
							vertices.push_back(t[1]+u[1]+v[1]);
							vertices.push_back(t[2]+u[2]+v[2]);

							vertices.push_back(t[0]+v[0]);
							vertices.push_back(t[1]+v[1]);
							vertices.push_back(t[2]+v[2]);
						}
					}
				}
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
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

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

void Chunk::delete_buffers()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}