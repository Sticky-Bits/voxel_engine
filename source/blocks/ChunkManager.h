#pragma once

#include <unordered_map>
#include <queue>
#include <functional>

#include <glm/glm.hpp>

#include "Chunk.h"

class ChunkManager
{
public:
	// Constructors
	ChunkManager(Renderer* p_renderer, Settings* p_settings);

	// Methods
	void change_chunks(glm::vec3 before, glm::vec3 after, bool initial_load = false);
	void show_chunk(glm::vec3 chunk, bool immediate = false);
	void hide_chunk(glm::vec3 chunk, bool immediate = false);
	void regenerate();

	// Queue Methods
	void process_chunk_queue();
	void process_entire_chunk_queue();

	// Vars
	static const int VIEW_DISTANCE = 10;
	std::queue<std::function<void()>> operation_queue;

	// A new world order
	void create_new_chunk(int x, int y, int z);
	void render();
	Renderer* mp_renderer;
	Settings* mp_settings;

private:
	// Methods
	void _show_chunk(glm::vec3 chunk);
	void _hide_chunk(glm::vec3 chunk);

};
