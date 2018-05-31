#include "Game.h"

Game* Game::mp_instance = 0;

Game* Game::get_instance()
{
	if (mp_instance == 0)
		mp_instance = new Game;
	return mp_instance;
}

void Game::create(Settings* settings)
{
	mp_settings = settings;
	mp_window = new Window(this, mp_settings);

	mp_window->create();

	m_should_quit = false;
	m_delta_time = 0.0f;

	mp_camera = new Camera(glm::vec3(0, 0, 0));
	mp_renderer = new Renderer(mp_camera, mp_window);
	mp_chunk_manager = new ChunkManager(mp_renderer, mp_settings);
}

void Game::destroy()
{
	if (mp_instance)
	{
		mp_window->destroy();
		delete mp_window;
		delete mp_instance;
	}
}

void Game::poll_events()
{
	mp_window->poll_events();
}

bool Game::should_close()
{
	return m_should_quit;
}

void Game::update()
{
	// Timing
	double current_frame = glfwGetTime();
	m_delta_time = current_frame - previous_frame;
	previous_frame = current_frame;

	// Update window
	mp_window->update(m_delta_time);
}

void Game::render()
{
	mp_renderer->pre_render();
	mp_chunk_manager->render();
	mp_renderer->post_render();
}

void Game::resize_window(int width, int height)
{
	m_window_width = width;
	m_window_height = height;

	mp_window->resize_window(m_window_width, m_window_height);
}

void Game::set_framebuffer_size(int width, int height)
{
	mp_renderer->set_framebuffer_size(width, height);
}

void Game::close_window()
{
	// TODO: Add "Are you sure" crap here
	m_should_quit = true;
}