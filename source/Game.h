#include "Settings.h"
#include "Window.h"
#include "blocks/ChunkManager.h"
#include "renderer/Camera.h"

class Game
{
public:
	static Game* get_instance();

	// Create game from given settings
	void create(Settings* settings);

	// Main Loop Calls
	bool should_close();
	void poll_events();
	void update();
	void render();
	void destroy();

	// Input handlers
	void key_pressed(int key, int scancode, int mods);
	void key_released(int key, int scancode, int mods);
	void character_entered(int key_code);
	void mouse_left_pressed();
	void mouse_right_pressed();
	void mouse_middle_pressed();
	void mouse_left_released();
	void mouse_right_released();
	void mouse_middle_released();
	void mouse_scroll(double x, double y);

	void resize_window(int width, int height);
	void close_window();

private:
	static Game* mp_instance;
	Camera * mp_camera;
	ChunkManager * mp_chunk_manager;
	Window* mp_window;
	Settings* mp_settings;
	Renderer* mp_renderer;

	bool m_should_quit;

	// Timing vars
	double m_delta_time;
	double current_frame;
	double previous_frame;

	int m_window_width;
	int m_window_height;
};
