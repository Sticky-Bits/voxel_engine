#include "Settings.h"

Settings::Settings()
{
	// NYI
}

Settings::~Settings()
{
	// NYI
}

void Settings::load_settings()
{
	// In the future, read settings from a file. For now just load defaults.
	m_window_width = 800;
	m_window_height = 600;
	m_full_screen = false;
}

void Settings::save_settings()
{
	// NYI
}