#pragma once

class Settings
{
public:
	Settings();
	~Settings();

	void load_settings();
	void save_settings();

	int m_window_width;
	int m_window_height;
	bool m_full_screen;
};