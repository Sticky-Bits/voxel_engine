#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <queue>
#include <functional>

#include "../Util.h"

#include "ChunkManager.h"
#include "../Settings.h"
#include "../Game.h"

ChunkManager::ChunkManager(Renderer* p_renderer, Settings* p_settings)
{
	// TODO: Maybe don't do the nines thing...
	mp_renderer = p_renderer;
	mp_settings = p_settings;
	create_new_chunk(0, 0, 0);
}

void ChunkManager::regenerate()
{
	std::queue<std::function<void()>> empty;
	std::swap(operation_queue, empty);
	change_chunks(glm::vec3(999.0f, 999.0f, 999.0f), glm::vec3(0.0f, 0.0f, 0.0f), true);
}

void ChunkManager::change_chunks(glm::vec3 before, glm::vec3 after, bool initial_load)
{
	std::set<glm::vec3, compareVec> before_set, after_set;
	for (int dx = -VIEW_DISTANCE; dx < VIEW_DISTANCE + 1; dx++)
	{
		for (int dy = -2; dy < 2; dy++) /*(int dy = -VIEW_DISTANCE; dy < VIEW_DISTANCE + 1; dy++)*/
		{
			for (int dz = -VIEW_DISTANCE; dz < VIEW_DISTANCE + 1; dz++)
			{
				// Constrain to radius, instead of manhattan distance
				if (pow(dx, 2) + pow(dy, 2) + pow(dz, 2) > pow((VIEW_DISTANCE + 1), 2))
				{
					continue;
				}
				before_set.insert(glm::vec3(before.x + dx, before.y + dy, before.z + dz));
				after_set.insert(glm::vec3(after.x + dx, after.y + dy, after.z + dz));
			}
		}
	}

	std::vector<glm::vec3> show_set, hide_set;

	std::set_difference(after_set.begin(), after_set.end(), before_set.begin(), before_set.end(), std::inserter(show_set, show_set.begin()), compareVec());
	std::set_difference(before_set.begin(), before_set.end(), after_set.begin(), after_set.end(), std::inserter(hide_set, hide_set.end()), compareVec());

	// Schedule chunks for loading / unloading
	for (auto chunk : show_set)
	{
		show_chunk(chunk);
	}

	if (!initial_load)
	{
		for (auto chunk : hide_set)
		{
			hide_chunk(chunk);
		}
	}
}

void ChunkManager::show_chunk(glm::vec3 chunk, bool immediate)
{
	if (immediate)
	{
		_show_chunk(chunk);
	}
	else
	{
		std::function<void()> function = std::bind(&ChunkManager::_show_chunk, this, chunk);
		operation_queue.push(function);
	}
}

void ChunkManager::hide_chunk(glm::vec3 chunk, bool immediate)
{
	if (immediate)
	{
		_hide_chunk(chunk);
	}
	else
	{
		std::function<void()> function = std::bind(&ChunkManager::_hide_chunk, this, chunk);
		operation_queue.push(function);
	}
}

void ChunkManager::create_new_chunk(int x, int y, int z)
{
	Chunk* p_new_chunk = new Chunk(mp_renderer, this, mp_settings);
	p_new_chunk->set_position(glm::vec3((float)x, (float)y, (float)z) * (float)Chunk::CHUNK_SIZE);
	p_new_chunk->setup();
}

void ChunkManager::render()
{
	//for chunk in chunks: chunk.render()
}

void ChunkManager::_show_chunk(glm::vec3 chunk)
{
	// Create pair of position_3d and Chunk called insert_chunk, consisting of position_3d = passed param 'chunk' and a new Chunk object at position 'chunk'
	//std::pair<glm::vec3, Chunk> insert_chunk(chunk, Chunk(chunk));
	//chunks.insert(insert_chunk);
}

void ChunkManager::_hide_chunk(glm::vec3 chunk)
{
}

void ChunkManager::process_chunk_queue()
{
	std::function<void()> func = operation_queue.front();
	operation_queue.pop();
	func();
}

void ChunkManager::process_entire_chunk_queue()
{
	while (!operation_queue.empty())
		process_chunk_queue();
}