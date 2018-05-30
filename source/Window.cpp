#include <stdlib.h>
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Game.h"
#include "Settings.h"

#include <iostream>

// Callbacks
void window_resize_callback(GLFWwindow* p_window, int width, int height);
void window_close_callback(GLFWwindow* p_window);
void key_callback(GLFWwindow* p_window, int key, int scancode, int action, int mods);
void character_callback(GLFWwindow* p_window, unsigned int key_code);
void mouse_button_callback(GLFWwindow* p_window, int button, int action, int mods);
void mouse_scroll_callback(GLFWwindow* p_window, double x, double y);

Window::Window(Game* p_game, Settings* p_settings)
{
	mp_game = p_game;
	mp_settings = p_settings;

	m_cursor_x = 0;
	m_cursor_y = 0;

	// Set default window dimensions
	m_window_width = mp_settings->m_window_width;
	m_window_height = mp_settings->m_window_height;
	m_old_window_width = m_window_width;
	m_old_window_height = m_window_height;
}

Window::~Window()
{

}

void Window::create()
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	// GLFW Settings
	// TODO: Maybe add MSAA to settings?
	int samples = 8;
	glfwWindowHint(GLFW_SAMPLES, samples);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create GLFW window
	mp_window = glfwCreateWindow(m_window_width, m_window_height, "Voxel Engine", NULL, NULL);
	if (!mp_window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	initialise_window_context(mp_window);
	std::cout << "Initialised GL context" << std::endl;

	// Load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (mp_settings->m_full_screen)
	{
		set_full_screen(true);
	}
}

void Window::destroy()
{
	glfwTerminate();
}

void Window::update(double dt)
{
	double x;
	double y;
	glfwGetCursorPos(mp_window, &x, &y);
	m_cursor_x = (int)floor(x);
	m_cursor_y = (int)floor(y);

}

void Window::set_full_screen(bool full_screen)
{
	if (full_screen)
	{
		const GLFWvidmode* p_video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		m_old_window_width = m_window_width;
		m_old_window_height = m_window_height;

		m_window_width = p_video_mode->width;
		m_window_height = p_video_mode->height;
	}
	else
	{
		m_window_width = m_old_window_width;
		m_window_height = m_old_window_height;
	}

	// TODO: Change name of window to local variable so we're not repeating the value.
	// Make new window
	GLFWwindow* p_new_window = glfwCreateWindow(m_window_width, m_window_height, "Voxel Engine", full_screen ? glfwGetPrimaryMonitor() : NULL, mp_window);
	initialise_window_context(p_new_window);

	// Destroy existing window and assign new one
	glfwDestroyWindow(mp_window);
	mp_window = p_new_window;
}

void Window::poll_events()
{
	glfwPollEvents();
}

void Window::initialise_window_context(GLFWwindow* p_window)
{
	glfwSetWindowSizeCallback(p_window, window_resize_callback);
	glfwSetWindowCloseCallback(p_window, window_close_callback);
	glfwSetKeyCallback(p_window, key_callback);
	glfwSetCharCallback(p_window, character_callback);
	glfwSetMouseButtonCallback(p_window, mouse_button_callback);
	glfwSetScrollCallback(p_window, mouse_scroll_callback);

	// Center on screen
	const GLFWvidmode* p_vid_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwGetWindowSize(p_window, &m_window_width, &m_window_height);
	glfwSetWindowPos(p_window, (p_vid_mode->width - m_window_width) / 2, (p_vid_mode->height - m_window_height) / 2);

	// Make window's context current
	glfwMakeContextCurrent(p_window);

	// Force resize
	window_resize_callback(p_window, m_window_width, m_window_height);

	// Show window
	glfwShowWindow(p_window);
}

void Window::resize_window(int width, int height)
{
	m_window_width = width;
	m_window_height = height;
}

void window_resize_callback(GLFWwindow* p_window, int width, int height)
{
	Game::get_instance()->resize_window(width, height);
}

void window_close_callback(GLFWwindow* p_window)
{
	Game::get_instance()->close_window();
}