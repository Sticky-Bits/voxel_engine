#include <stdlib.h>

#include "Game.h"

int main()
{
	// Load settings
	Settings* p_settings = new Settings();
	p_settings->load_settings();

	// Create game using settings
	Game* p_game = Game::get_instance();
	p_game->create(p_settings);

	// Main loop
	while (!p_game->should_close())
	{
		// Poll input events
		p_game->poll_events();

		// Update
		p_game->update();

		// Render
		p_game->render();
	}

	// Destroy objects, window etc.
	p_game->destroy();

	// Exit
	exit(EXIT_SUCCESS);

} 