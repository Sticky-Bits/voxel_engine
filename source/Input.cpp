#include "Game.h"

// Input callbacks
void key_callback(GLFWwindow* p_window, int key, int scancode, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
	{
		Game::get_instance()->key_pressed(key, scancode, mods);
		break;
	}
	case GLFW_RELEASE:
	{
		Game::get_instance()->key_released(key, scancode, mods);
		break;
	}
	case GLFW_REPEAT:
	{
		break;
	}
	}
}

void character_callback(GLFWwindow* p_window, unsigned int key_code)
{
	Game::get_instance()->character_entered(key_code);
}

void mouse_button_callback(GLFWwindow* p_window, int button, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			Game::get_instance()->mouse_left_pressed();
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
			Game::get_instance()->mouse_right_pressed();
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			Game::get_instance()->mouse_middle_pressed();

		break;
	}
	case GLFW_RELEASE:
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			Game::get_instance()->mouse_left_released();
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
			Game::get_instance()->mouse_right_released();
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			Game::get_instance()->mouse_middle_released();

		break;
	}
	}
}

void mouse_scroll_callback(GLFWwindow* p_window, double x, double y)
{
	Game::get_instance()->mouse_scroll(x, y);
}

void Game::key_pressed(int key, int scancode, int mods)
{

}

void Game::key_released(int key, int scancode, int mods)
{

}

void Game::character_entered(int keycode)
{
	// Pass to GUI
}

void Game::mouse_left_pressed()
{

}

void Game::mouse_right_pressed()
{

}

void Game::mouse_middle_pressed()
{

}

void Game::mouse_left_released()
{

}

void Game::mouse_right_released()
{

}

void Game::mouse_middle_released()
{

}

void Game::mouse_scroll(double x, double y)
{

}