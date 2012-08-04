/***************************************************************************
 * video.h
 *
 * Copyright (C) 2005 - 2011 Florian Richter
 ***************************************************************************/
/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SMC_VIDEO_H
#define SMC_VIDEO_H

#include "../core/global_basic.h"
#include "../core/global_game.h"
#include "../video/color.h"
// glx
#ifdef __unix__
	#include <GL/glx.h>
#endif
// SDL
#include "SDL.h"
#include "SDL_image.h"
#ifdef __unix__
	#define NO_SDL_GLEXT
#endif
#include "SDL_opengl.h"
#ifdef __unix__
	#undef NO_SDL_GLEXT
#endif
#include "SDL_syswm.h"
// CEGUI
#include "CEGUISystem.h"
#include "RendererModules/OpenGL/CEGUIOpenGLRenderer.h"
// boost thread
#include <boost/thread/thread.hpp>

namespace SMC
{

/* *** *** *** *** *** *** *** Effect types *** *** *** *** *** *** *** *** *** *** */

enum Effect_Fadeout
{
	EFFECT_OUT_RANDOM,
	// fade out the screen to black
	EFFECT_OUT_BLACK,
	// black color gradient rectangles moving to the middle from left/right or up/down
	EFFECT_OUT_HORIZONTAL_VERTICAL,
	// an item slowly moves into the screen
	EFFECT_OUT_BIG_ITEM,
	// changes the screen to a random color while fading out
	EFFECT_OUT_RANDOM_COLOR_BOOST,
	// big black rotating rectangles pop up somewhat randomly on a tiled grid
	EFFECT_OUT_BLACK_TILED_RECTS,
	// small fast random alpha color boxes fades out the screen
	EFFECT_OUT_FIXED_COLORBOX,
	EFFECT_OUT_AMOUNT
};

enum Effect_Fadein
{
	EFFECT_IN_RANDOM,
	EFFECT_IN_BLACK, // fade in the screen from black
	EFFECT_IN_AMOUNT
};

/* *** *** *** *** *** *** *** Video class *** *** *** *** *** *** *** *** *** *** */

class cVideo
{
public:
	cVideo( void );
	~cVideo( void );

	// Initialize CEGUI system and renderer fake for the preferences xml parser
	void Init_CEGUI_Fake( void ) const;
	// Delete the CEGUI system and renderer fake
	void Delete_CEGUI_Fake( void ) const;
	// Initialize the CEGUI System and Renderer
	void Init_CEGUI( void ) const;
	// Initialize the basic CEGUI data and configuration
	void Init_CEGUI_Data( void ) const;
	// Initialize all the SDL systems
	void Init_SDL( void );
	/* Initialize the screen surface
	 * reload_textures_from_file: if set reloads all textures from the original file
	 * use_preferences: if set use user preferences settings
	 * shows an error if failed and exits
	*/
	void Init_Video( bool reload_textures_from_file = 0, bool use_preferences = 1 );
	// Initialize OpenGL with current settings
	void Init_OpenGL( void );
	// Initialize Geometry with current settings
	void Init_Geometry( void );
	// Initialize Texture detail settings
	void Init_Texture_Detail( void );
	// initialize the up/down scaling value for the current resolution ( image/mouse scale )
	void Init_Resolution_Scale( void ) const;

	/* Test if the given resolution and bits per pixel are valid
	 * if flags aren't set they are auto set from the preferences
	 * returns 0 if the requested mode is not supported under any bit depth,
	 * or returns the bits-per-pixel of the closest available mode
	*/
	int Test_Video( int width, int height, int bpp, int flags = 0 ) const;

	/* Get the supported resolutions for the current screen pixel format
	 * if flags aren't set they are set to the default
	*/
	vector<cSize_Int> Get_Supported_Resolutions( int flags = 0 ) const;

	// make the opengl context active for the current thread
	void Make_GL_Context_Current( void );
	// make the opengl context inactive for the current thread
	void Make_GL_Context_Inactive( void );

	// Render game, GUI and swap the opengl buffer
	void Render();
	
	/* Check if the image was already loaded and returns a pointer to it else it will be loaded
	 * The returned image should not be deleted or modified.
	 */
	cGL_Surface *Get_Surface( std::string filename, bool print_errors = 1 );

	// Software image
	class cSoftware_Image
	{
	public:
		cSoftware_Image( void )
		{
			m_sdl_surface = NULL;
			m_settings = NULL;
		};

		SDL_Surface *m_sdl_surface;
		cImage_Settings_Data *m_settings;
	};

	/* Load and return the software image with the settings data
	 * The returned sdl image should be deleted if not used anymore but not the settings data which is managed
	 * load_settings : enable file settings if set to 1
	 * print_errors : print errors if image couldn't be created or loaded
	*/
	cSoftware_Image Load_Image( std::string filename, bool load_settings = 1, bool print_errors = 1 ) const;

	/* Load and return the hardware image
	 * use_settings : enable file settings if set to 1
	 * print_errors : print errors if image couldn't be created or loaded
	 * The returned image should be deleted if not used anymore
	*/
	cGL_Surface *Load_GL_Surface( std::string filename, bool use_settings = 1, bool print_errors = 1 );

	/* Convert to a scaled software image with a power of 2 size and 32 bits per pixel.
	 * Conversion only happens if needed.
	 * surface : the source image which gets converted if needed
	 * only use the returned image after this conversion
	*/
	SDL_Surface *Convert_To_Final_Software_Image( SDL_Surface *surface ) const;

	/* Convert a SDL_Surface to a GL image
	 * surface : the source SDL_surface which will be auto-deleted.
	 * mipmap : create texture mipmaps
	 * force_width/height : force the given width and height
	*/
	cGL_Surface *Create_Texture( SDL_Surface *surface, bool mipmap = 0, unsigned int force_width = 0, unsigned int force_height = 0 ) const;

	/* Copy pixels to the bound GL texture
	 * mipmap : create texture mipmaps
	*/
	void Create_GL_Texture( unsigned int width, unsigned int height, const void *pixels, bool mipmap = 0 ) const;

	// Get pixel color of the given position on the screen
	Color Get_Pixel( int x, int y ) const;

	// Reset and clear the screen
	void Clear_Screen( void ) const;
	/* Draw a rectangle
	 * if request is NULL automatically creates the request
	*/
	void Draw_Rect( const GL_rect *rect, float z, const Color *color, cRect_Request *request = NULL ) const;
	void Draw_Rect( float x, float y, float width, float height, float z, const Color *color, cRect_Request *request = NULL ) const;
	/* Draw a gradient
	 * if request is NULL automatically creates the request
	*/
	void Draw_Gradient( const GL_rect *rect, float z, const Color *color_1, const Color *color_2, ObjectDirection direction, cGradient_Request *request = NULL ) const;
	void Draw_Gradient( float x, float y, float width, float height, float z, const Color *color_1, const Color *color_2, ObjectDirection direction, cGradient_Request *request = NULL ) const;
	// Draw a circle
	//void Draw_Circle( GL_circle *circle, float z, Color *color, cCircleRequest *request = NULL );
	void Draw_Circle( float x, float y, float radius, float z, const Color *color, cCircle_Request *request = NULL ) const;
	// Draw a line
	void Draw_Line( const GL_line *line, float z, const Color *color, cLine_Request *request = NULL ) const;
	void Draw_Line( float x1, float y1, float x2, float y2, float z, const Color *color, cLine_Request *request = NULL ) const;

	/* Return the scale size if the image is bigger as the given size
	 * also upscales if only_downscale is set to 0
	*/
	float Get_Scale( const cGL_Surface *image, float width, float height, bool only_downscale = 1 ) const;
	// scale the size down if the width or height is bigger than the maximum supported texture size
	void Apply_Max_Texture_Size( int &width, int &height ) const;

	/* Downscale an image
	 * Can be used for creating MIPmaps
	 * The incoming image should have a power-of-two size
	*/
	bool Downscale_Image( const unsigned char *const orig, int width, int height, int channels, unsigned char *resampled, int block_size_x, int block_size_y ) const;

	// available OpenGL version
	float m_opengl_version;

	// using double buffering
	bool m_double_buffer;

	// screen red, green and blue color bit size
	int m_rgb_size[3];

	// default drawing buffer
	GLint m_default_buffer;
	// max texture size
	GLint m_max_texture_size;

	// if audio initialization failed
	bool m_audio_init_failed;
	// if joystick initialization failed
	bool m_joy_init_failed;

	// active image cache directory
	std::string m_imgcache_dir;

	// geometry quality level 0.0 - 1.0
	float m_geometry_quality;
	// texture quality level 0.0 - 1.0
	float m_texture_quality;

	// window manager information
	SDL_SysWMinfo wm_info;
#ifdef __unix__
	// current opengl context
	GLXContext glx_context;
#endif
	// rendering thread
	boost::thread m_render_thread;

private:
	// if set video is initialized successfully
	bool m_initialised;
};

/* Draw an Screen Fadeout Effect
 * if effect is RANDOM_EFFECT a random effect is selected
*/
void Draw_Effect_Out( Effect_Fadeout effect = EFFECT_OUT_RANDOM, float speed = 1 );

/* Draw an Screen Fadein Effect
 * if effect is RANDOM_EFFECT a random effect is selected
*/
void Draw_Effect_In( Effect_Fadein effect = EFFECT_IN_RANDOM, float speed = 1 );

// initialize loading screen
void Loading_Screen_Init( void );
// set the loading screen info string and draw it
void Loading_Screen_Draw_Text( const std::string &str_info = "Loading" );
// draw the loading screen
void Loading_Screen_Draw( void );
// exit loading screen
void Loading_Screen_Exit( void );

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

// Video Handler
extern cVideo *pVideo;

// GUI System
extern CEGUI::OpenGLRenderer *pGuiRenderer;
extern CEGUI::System *pGuiSystem;

// Screen
extern SDL_Surface *screen;

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC

#endif
