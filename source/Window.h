#pragma once

#include <GLFW\glfw3.h>

class Game;
class Settings;

class Window
{
public:
	Window(Game* p_game, Settings* p_settings);
	~Window();

	void create();
	void destroy();
	void update(double dt);
	void render();

	void initialise_window_context(GLFWwindow* p_window);

	int get_window_height();
	int get_window_width();
	void resize_window(int width, int height);
	void set_full_screen(bool full_screen);
	void poll_events();

	int m_window_width;
	int m_window_height;

private:
	GLFWwindow* mp_window;
	Game* mp_game;
	Settings* mp_settings;

	int m_old_window_width;
	int m_old_window_height;

	int m_cursor_x;
	int m_cursor_y;

};