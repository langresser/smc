/***************************************************************************
 * font.cpp  -  internal font functions
 *
 * Copyright (C) 2006 - 2011 Florian Richter
 ***************************************************************************/
/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
 
#include "../video/font.h"
#include "../video/gl_surface.h"

namespace SMC
{

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

void Font_Delete_Ref( cGL_Surface *surface )
{
	pFont->Delete_Ref( surface );
}

/* *** *** *** *** *** *** *** Font Manager class *** *** *** *** *** *** *** *** *** *** */

cFont_Manager :: cFont_Manager( void )
{
	m_font_normal = NULL;
	m_font_small = NULL;
	m_font_very_small = NULL;
}

cFont_Manager :: ~cFont_Manager( void )
{
	// if not initialized
	if( !TTF_WasInit() )
	{
		return;
	}

	if( m_font_normal )
	{
		TTF_CloseFont( m_font_normal );
		m_font_normal = NULL;
	}

	if( m_font_small )
	{
		TTF_CloseFont( m_font_small );
		m_font_small = NULL;
	}

	if( m_font_very_small )
	{
		TTF_CloseFont( m_font_very_small );
		m_font_very_small = NULL;
	}

	TTF_Quit();
}

void cFont_Manager :: Init( void )
{
	// if already initialised
	if( TTF_WasInit() )
	{
		return;
	}
	
	// init ttf
	if( TTF_Init() == -1 )
	{
		printf( "Error : SDL_TTF initialization failed\nReason : %s\n", SDL_GetError() );
		exit( EXIT_FAILURE );
	}

	// open fonts
	m_font_normal = TTF_OpenFont( DATA_DIR "/" GUI_FONT_DIR "/default_bold.ttf", 18 );
	m_font_small = TTF_OpenFont( DATA_DIR "/" GUI_FONT_DIR "/default_bold.ttf", 11 );
	m_font_very_small = TTF_OpenFont( DATA_DIR "/" GUI_FONT_DIR "/default_bold.ttf", 9 );

	// if loading failed
	if( !m_font_normal || !m_font_small || !m_font_very_small )
	{
		printf( "Error : Font loading failed from directory %s\n", GUI_FONT_DIR );
		exit( EXIT_FAILURE );
	}
}

void cFont_Manager :: Add_Ref( cGL_Surface *surface )
{
	if( !surface )
	{
		return;
	}

	m_active_fonts.push_back( surface );
}

void cFont_Manager :: Delete_Ref( cGL_Surface *surface )
{
	for( ActiveFontList::iterator itr = m_active_fonts.begin(); itr != m_active_fonts.end(); ++itr )
	{
		cGL_Surface *obj = (*itr);

		// delete reference if found
		if( obj == surface )
		{
			m_active_fonts.erase( itr );
			return;
		}
	}
}

cGL_Surface *cFont_Manager :: Render_Text( TTF_Font *font, const std::string &text, const Color color )
{
	// get SDL Color
	SDL_Color sdlcolor = color.Get_SDL_Color();
	// create text surface
	cGL_Surface *surface = pVideo->Create_Texture( TTF_RenderUTF8_Blended( font, text.c_str(), sdlcolor ) );

	if( !surface )
	{
		return NULL;
	}

	surface->m_filename = text;

	// set function if font gets deleted
	surface->Set_Destruction_Function( &Font_Delete_Ref );
	// add font to active fonts
	Add_Ref( surface );
	
	return surface;
}

void cFont_Manager :: Restore_Textures( void )
{
	// load back into hardware textures
	for( Saved_Texture_List::iterator itr = m_software_textures.begin(); itr != m_software_textures.end(); ++itr )
	{
		// get saved texture
		cSaved_Texture *soft_tex = (*itr);
		// load it
		soft_tex->m_base->Load_Software_Texture( soft_tex );
	}

	// delete software textures
	for( Saved_Texture_List::iterator itr = m_software_textures.begin(); itr != m_software_textures.end(); ++itr )
	{
		delete *itr;
	}

	m_software_textures.clear();
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

cFont_Manager *pFont = NULL;

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
