/***************************************************************************
 * gl_surface.h
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

#ifndef SMC_GL_SURFACE_H
#define SMC_GL_SURFACE_H

#include "../core/global_basic.h"
#include "../core/math/point.h"
// SDL
#include "SDL.h"

#ifdef WIN32
#include "SDL_opengl.h"
#else
#include "SDL_opengles.h"
#endif

namespace SMC
{

/* *** *** *** *** *** *** *** *** OpenGL Surface *** *** *** *** *** *** *** *** *** */

class cGL_Surface
{
public:
	cGL_Surface( void );
	~cGL_Surface( void );

	/* Blit the surface on the given position
	 * if request is NULL automatically creates the request
	*/
	void Blit( float x, float y, float z, cSurface_Request *request = NULL ) const;
	// Blit only the surface data on the given request
	void Blit_Data( cSurface_Request *request ) const;

	// Copy cGL_Surface and return it
	cGL_Surface *Copy( void ) const;
	
	// Set the ground type
	void Set_Ground_Type( GroundType gtype );

	// Check if the OpenGL texture is used by another cGL_Surface
	bool Is_Texture_Use_Multiple( void ) const;

	/* Return a software texture copy
	 * only_filename: if set doesn't save the software texture but only the filename
	*/
	cSaved_Texture *Get_Software_Texture( bool only_filename = 0 );
	// Load a software texture
	void Load_Software_Texture( cSaved_Texture *soft_tex );

	// Return the filename
	std::string Get_Filename( int with_dir = 2, bool with_end = 1 ) const;
	// Set a function called on destruction
	void Set_Destruction_Function( void ( *nfunction )( cGL_Surface * ) );

	// GL texture number
	GLuint m_image;
	// internal drawing offset
	float m_int_x;
	float m_int_y;
	// starting drawing dimension without modifications like rotation and scaling
	float m_start_w;
	float m_start_h;
	// final drawing dimension
	float m_w;
	float m_h;
	// texture dimension
	unsigned int m_tex_w;
	unsigned int m_tex_h;
	// internal rotation
	float m_base_rot_x;
	float m_base_rot_y;
	float m_base_rot_z;
	// image collision data
	GL_point m_col_pos;
	float m_col_w;
	float m_col_h;

	// origin if created from a file
	std::string m_filename;
	// should the image be deleted
	bool m_auto_del_img;
	// if managed over the image manager
	bool m_managed;
	// if the image is tagged as obsolete 
	bool m_obsolete;

	// editor tags
	std::string m_editor_tags;
	// name
	std::string m_name;
	// default sprite type
	unsigned int m_type;
	// ground type
	GroundType m_ground_type;
private:
	// function called on destruction
	void ( *destruction_function )( cGL_Surface * );
};

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC

#endif
