/***************************************************************************
 * preferences.h
 *
 * Copyright (C) 2003 - 2011 Florian Richter
 ***************************************************************************/
/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SMC_PREFERENCES_H
#define SMC_PREFERENCES_H

#include "../core/global_basic.h"
// SDL
#include "SDL.h"
// CEGUI
#include "CEGUIXMLHandler.h"
#include "CEGUIXMLAttributes.h"

namespace SMC
{

/* *** *** *** *** *** cPreferences *** *** *** *** *** *** *** *** *** *** *** *** */

class cPreferences : public CEGUI::XMLHandler
{
public:
	cPreferences( void );
	virtual ~cPreferences( void );
	
	// Load the preferences from a file
	bool Load( const std::string &filename = "" );
	// Save the preferences to a file
	void Save( void );

	// Reset the settings
	void Reset_All( void );
	void Reset_Game( void );
	void Reset_Video( void );
	void Reset_Audio( void );
	void Reset_Keyboard( void );
	void Reset_Editor( void );
	 // Get settings from current game settings
	void Update( void );
	// Set settings to current game settings
	void Apply( void );
	// Set new video settings
	void Apply_Video( Uint16 screen_w, Uint16 screen_h, Uint8 screen_bpp, bool fullscreen, bool vsync, float geometry_detail, float texture_detail );
	// Set new audio settings
	void Apply_Audio( bool sound, bool music );

	/* *** *** *** Settings *** *** *** *** */

	// Game
	// last version of smc which saved the preferences file
	unsigned int m_game_version;
	// default language
	std::string m_language;
	// player always runs
	bool m_always_run;
	// menu level name to load
	std::string m_menu_level;
	// smart camera speed
	float m_camera_hor_speed;
	float m_camera_ver_speed;

	// Audio
	bool m_audio_music;
	bool m_audio_sound;

	// Keyboard
	// key definitions
	SDLKey m_key_up;
	SDLKey m_key_down;
	SDLKey m_key_left;
	SDLKey m_key_right;
	SDLKey m_key_jump;
	SDLKey m_key_shoot;
	SDLKey m_key_item;
	SDLKey m_key_action;
	SDLKey m_key_screenshot;
	SDLKey m_key_editor_fast_copy_up;
	SDLKey m_key_editor_fast_copy_down;
	SDLKey m_key_editor_fast_copy_left;
	SDLKey m_key_editor_fast_copy_right;
	SDLKey m_key_editor_pixel_move_up;
	SDLKey m_key_editor_pixel_move_down;
	SDLKey m_key_editor_pixel_move_left;
	SDLKey m_key_editor_pixel_move_right;
	// scroll speed
	float m_scroll_speed;

	// Editor
	// hide mouse if clicked on an object
	bool m_editor_mouse_auto_hide;
	// show item images
	bool m_editor_show_item_images;
	// size of the item images
	unsigned int m_editor_item_image_size;

	// Special
	// level background images enabled
	bool m_level_background_images;
	// image cache enabled
	bool m_image_cache_enabled;

	/* *** *** *** *** *** *** *** */

	// configuration filename
	std::string m_config_filename;

	/* *** *** *** defaults *** *** *** *** */
	// Game
	static const bool m_always_run_default;
	static const std::string m_menu_level_default;
	static const float m_camera_hor_speed_default;
	static const float m_camera_ver_speed_default;
	// Audio
	static const bool m_audio_music_default;
	static const bool m_audio_sound_default;
	static const unsigned int m_audio_hz_default;
	static const Uint8 m_sound_volume_default;
	static const Uint8 m_music_volume_default;
	// Video
	static const bool m_video_fullscreen_default;
	static const Uint16 m_video_screen_w_default;
	static const Uint16 m_video_screen_h_default;
	static const Uint8 m_video_screen_bpp_default;
	static const bool m_video_vsync_default;
	static const Uint16 m_video_fps_limit_default;
	static const float m_geometry_quality_default;
	static const float m_texture_quality_default;
	// Keyboard
	static const SDLKey m_key_up_default;
	static const SDLKey m_key_down_default;
	static const SDLKey m_key_left_default;
	static const SDLKey m_key_right_default;
	static const SDLKey m_key_jump_default;
	static const SDLKey m_key_shoot_default;
	static const SDLKey m_key_item_default;
	static const SDLKey m_key_action_default;
	static const SDLKey m_key_screenshot_default;
	static const SDLKey m_key_editor_fast_copy_up_default;
	static const SDLKey m_key_editor_fast_copy_down_default;
	static const SDLKey m_key_editor_fast_copy_left_default;
	static const SDLKey m_key_editor_fast_copy_right_default;
	static const SDLKey m_key_editor_pixel_move_up_default;
	static const SDLKey m_key_editor_pixel_move_down_default;
	static const SDLKey m_key_editor_pixel_move_left_default;
	static const SDLKey m_key_editor_pixel_move_right_default;
	static const float m_scroll_speed_default;
	// Joystick
	static const bool m_joy_enabled_default;
	static const bool m_joy_analog_jump_default;
	static const int m_joy_axis_hor_default;
	static const int m_joy_axis_ver_default;
	static const Sint16 m_joy_axis_threshold_default;
	static const Uint8 m_joy_button_jump_default;
	static const Uint8 m_joy_button_shoot_default;
	static const Uint8 m_joy_button_item_default;
	static const Uint8 m_joy_button_action_default;
	static const Uint8 m_joy_button_exit_default;
	// Editor
	static const bool m_editor_mouse_auto_hide_default;
	static const bool m_editor_show_item_images_default;
	static const unsigned int m_editor_item_image_size_default;

private:
	// XML element start
	virtual void elementStart( const CEGUI::String &element, const CEGUI::XMLAttributes &attributes );
	// XML element end
	virtual void elementEnd( const CEGUI::String &element );
	// handles an item
	void handle_item( CEGUI::XMLAttributes attributes );
};

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

// The Preferences
extern cPreferences *pPreferences;

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC

#endif
