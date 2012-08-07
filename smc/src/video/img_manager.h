/***************************************************************************
 * img_manager.h
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

#ifndef SMC_IMG_MANAGER_H
#define SMC_IMG_MANAGER_H

#include "../core/global_basic.h"
#include "../video/video.h"
#include "../core/obj_manager.h"
#include "../video/gl_surface.h"

namespace SMC
{

/* *** *** *** *** *** cSaved_Texture *** *** *** *** *** *** *** *** *** *** *** *** */

// software texture data
class cSaved_Texture
{
public:
	cSaved_Texture( void );
	~cSaved_Texture( void );

	// base surface
	cGL_Surface *m_base;

	// pixel data
	GLubyte *m_pixels;

	// size
	GLint m_width;
	GLint m_height;
	// format
	GLint m_format;

	// settings
	GLint m_min_filter;
	GLint m_mag_filter;
	GLint m_wrap_s;
	GLint m_wrap_t;
};

typedef vector<cSaved_Texture *> Saved_Texture_List;
typedef vector<cGL_Surface *> GL_Surface_List;

/* *** *** *** *** *** *** cImage_Manager *** *** *** *** *** *** *** *** *** *** *** */

//  Keeps track of all the images in memory
//
// Operators:
//  - cImage_Manager [path]
//  - cImage_Manager [identifier]
class cImage_Manager : public cObject_Manager<cGL_Surface>
{
public:
	cImage_Manager( void );
	virtual ~cImage_Manager( void );

	// Add a surface
	virtual void Add( cGL_Surface *obj );

	// Return the surface by path
	cGL_Surface *Get_Pointer( const std::string &path ) const;

	// Return the copied image
	cGL_Surface *Copy( const std::string &path );

	cGL_Surface *operator [] ( unsigned int identifier )
	{
		return cObject_Manager<cGL_Surface>::Get_Pointer( identifier );
	}

	cGL_Surface *operator [] ( const std::string &path )
	{
		return Get_Pointer( path );
	}

	/* Load the saved software textures back into hardware textures or
	 * load textures from file if set
	 * draw_gui : if set use the loading screen gui for drawing
	*/
	void Restore_Textures( bool draw_gui = 0 );

	// Delete all surface textures, but keep object vector entries
	void Delete_Image_Textures( void );

	// Delete all hardware surfaces
	void Delete_Hardware_Textures( void );

	// Delete all Surfaces
	virtual void Delete_All( void );

	// highest opengl texture id found
	GLuint m_high_texture_id;

private:
	// saved textures for reloading
	Saved_Texture_List m_saved_textures;
};

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

// Image Manager
extern cImage_Manager *pImage_Manager;

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC

#endif
