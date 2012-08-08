/***************************************************************************
 * renderer.cpp  -  Render Queueing
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

#include "../video/renderer.h"
#include "../core/game_core.h"
#include <algorithm>

namespace SMC
{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const float doubled_pi = static_cast<float>(M_PI * 2.0f);
static GLuint last_bind_texture = 0;

/* *** *** *** *** *** *** cRender_Request *** *** *** *** *** *** *** *** *** *** *** */

cRender_Request :: cRender_Request( void )
{
	m_type = REND_NOTHING;
	m_pos_z = 0.0f;
	m_render_count = 1;
}

cRender_Request :: ~cRender_Request( void )
{

}

void cRender_Request :: Draw( void )
{
	// virtual
}

/* *** *** *** *** *** *** cClear_Request *** *** *** *** *** *** *** *** *** *** *** */

cClear_Request :: cClear_Request( void )
: cRender_Request()
{
	m_type = REND_CLEAR;
}

cClear_Request :: ~cClear_Request( void )
{

}

void cClear_Request :: Draw( void )
{
	// clear screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	// clear the matrix (default position and orientation)
	glLoadIdentity();
}

/* *** *** *** *** *** *** cRender_Request_Advanced *** *** *** *** *** *** *** *** *** *** *** */

cRender_Request_Advanced :: cRender_Request_Advanced( void )
: cRender_Request()
{
	m_global_scale = 1;
	m_no_camera = 1;

	m_rot_x = 0.0f;
	m_rot_y = 0.0f;
	m_rot_z = 0.0f;

	m_blend_sfactor = GL_SRC_ALPHA;
	m_blend_dfactor = GL_ONE_MINUS_SRC_ALPHA;

	m_shadow_pos = 0.0f;
	m_shadow_color = static_cast<Uint8>(0);

	m_combine_type = 0;
	m_combine_color[0] = 0.0f;
	m_combine_color[1] = 0.0f;
	m_combine_color[2] = 0.0f;
}

cRender_Request_Advanced :: ~cRender_Request_Advanced( void )
{

}

void cRender_Request_Advanced :: Render_Basic( void )
{
	// tried to replace this with gl push and pop but that was a lot slower on a Radeon X850 Pro
	// clear the matrix (default position and orientation)
	glLoadIdentity();

	// global scale
	if( m_global_scale && ( global_upscalex != 1.0f || global_upscaley != 1.0f ) )
	{
		glScalef( global_upscalex, global_upscaley, 1.0f );
	}

	// blend factor
	if( m_blend_sfactor != GL_SRC_ALPHA || m_blend_dfactor != GL_ONE_MINUS_SRC_ALPHA )
	{
		glBlendFunc( m_blend_sfactor, m_blend_dfactor );
	}
}

void cRender_Request_Advanced :: Render_Basic_Clear( void ) const
{
	// clear blend factor
	if( m_blend_sfactor != GL_SRC_ALPHA || m_blend_dfactor != GL_ONE_MINUS_SRC_ALPHA )
	{
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}

	// if debug build check for errors
#ifdef _DEBUG
	// glGetError only saves one error flag
	GLenum error = glGetError();

	if( error != GL_NO_ERROR )
	{
		printf( "RenderRequest : GL Error found : %d\n", error);
	}
#endif
}

void cRender_Request_Advanced :: Render_Advanced( void )
{
	// rotation
	if( m_rot_x != 0.0f )
	{
		glRotatef( m_rot_x, 1.0f, 0.0f, 0.0f );
	}
	if( m_rot_y != 0.0f )
	{
		glRotatef( m_rot_y, 0.0f, 1.0f, 0.0f );
	}
	if( m_rot_z != 0.0f )
	{
		glRotatef( m_rot_z, 0.0f, 0.0f, 1.0f );
	}

	// Color Combine
	if( m_combine_type != 0 )
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, m_combine_type );
		glTexEnvi( GL_TEXTURE_ENV, GL_SRC0_RGB, GL_CONSTANT );
        glTexEnvi( GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE );
		glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, m_combine_color );
	}
}

void cRender_Request_Advanced :: Render_Advanced_Clear( void ) const
{
	// clear color modifications
	if( m_combine_type != 0 )
	{
		float col[3] = { 0.0f, 0.0f, 0.0f };
		glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, col );
		glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	}
}

/* *** *** *** *** *** *** cLine_Request *** *** *** *** *** *** *** *** *** *** *** */

cLine_Request :: cLine_Request( void )
: cRender_Request_Advanced()
{
	m_type = REND_LINE;
	m_color = static_cast<Uint8>(0);
	m_line = GL_line( 0.0f, 0.0f, 0.0f, 0.0f );
	m_line_width = 1.0f;
	m_stipple_pattern = 0;
}

cLine_Request :: ~cLine_Request( void )
{

}

void cLine_Request :: Draw( void )
{
	Render_Basic();

	// set camera position
	if( !m_no_camera )
	{
		glTranslatef( -pActive_Camera->m_x, -pActive_Camera->m_y, m_pos_z );
	}
	else
	{
		// only z position
		glTranslatef( 0.0f, 0.0f, m_pos_z );
	}

	Render_Advanced();

	// color
	if( m_color.red != 255 || m_color.green != 255 || m_color.blue != 255 || m_color.alpha != 255 )
	{
		glColor4ub( m_color.red, m_color.green, m_color.blue, m_color.alpha );
	}

	if( glIsEnabled( GL_TEXTURE_2D ) )
	{
		glDisable( GL_TEXTURE_2D );
	}

	// change width
	if( m_line_width != 1.0f )
	{
		glLineWidth( m_line_width );
	}
    
    GLfloat vertex2[]={m_line.m_x1, m_line.m_y1,
        m_line.m_x2, m_line.m_y2,};
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertex2);
    glDrawArrays(GL_LINES, 0, 2);
    glDisableClientState(GL_VERTEX_ARRAY);

	// clear stipple pattern
	// clear line width
	if( m_line_width != 1.0f )
	{
		glLineWidth( 1.0f );
	}

	// clear color
	if( m_color.red != 255 || m_color.green != 255 || m_color.blue != 255 || m_color.alpha != 255 )
	{
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	Render_Advanced_Clear();
	Render_Basic_Clear();
}

/* *** *** *** *** *** *** cRect_Request *** *** *** *** *** *** *** *** *** *** *** */

cRect_Request :: cRect_Request( void )
: cRender_Request_Advanced()
{
	m_type = REND_RECT;
	m_color = static_cast<Uint8>(0);

	m_rect = GL_rect( 0.0f, 0.0f, 0.0f, 0.0f );
	m_filled = 1;
	m_scale_x = 1.0f;
	m_scale_y = 1.0f;
	m_scale_z = 1.0f;

	m_line_width = 1.0f;
	m_stipple_pattern = 0;
}

cRect_Request :: ~cRect_Request( void )
{

}

void cRect_Request :: Draw( void )
{
	Render_Basic();

	// get half the size
	const float half_w = m_rect.m_w / 2;
	const float half_h = m_rect.m_h / 2;
	// position
	float final_pos_x = m_rect.m_x + ( half_w * m_scale_x );
	float final_pos_y = m_rect.m_y + ( half_h * m_scale_y );

	// set camera position
	if( !m_no_camera )
	{
		final_pos_x -= pActive_Camera->m_x;
		final_pos_y -= pActive_Camera->m_y;
	}

	glTranslatef( final_pos_x, final_pos_y, m_pos_z );

	// scale
	if( m_scale_x != 1.0f || m_scale_y != 1.0f || m_scale_z != 1.0f )
	{
		glScalef( m_scale_x, m_scale_y, m_scale_z );
	}

	Render_Advanced();

	// color
	if( m_color.red != 255 || m_color.green != 255 || m_color.blue != 255 || m_color.alpha != 255 )
	{
		glColor4ub( m_color.red, m_color.green, m_color.blue, m_color.alpha );
	}

	if( glIsEnabled( GL_TEXTURE_2D ) )
	{
		glDisable( GL_TEXTURE_2D );
	}
    
    // change width
    if( m_line_width != 1.0f )
    {
        glLineWidth( m_line_width );
    }

    GLfloat vertex2[]={-half_w, -half_h,
        half_w, -half_h,
        half_w, half_h,
        -half_w, half_h};
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, vertex2);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// clear stipple pattern
	if( m_stipple_pattern != 0 )
	{
	}

	// clear line width
	if( m_line_width != 1.0f )
	{
		glLineWidth( 1.0f );
	}

	// clear color
	if( m_color.red != 255 || m_color.green != 255 || m_color.blue != 255 || m_color.alpha != 255 )
	{
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	Render_Advanced_Clear();
	Render_Basic_Clear();
}

/* *** *** *** *** *** *** cGradient_Request *** *** *** *** *** *** *** *** *** *** *** */

cGradient_Request :: cGradient_Request( void )
: cRender_Request_Advanced()
{
	m_type = REND_GRADIENT;
	m_rect = GL_rect( 0.0f, 0.0f, 0.0f, 0.0f );
	m_dir = DIR_UNDEFINED;
	m_color_1 = static_cast<Uint8>(0);
	m_color_2 = static_cast<Uint8>(0);
}

cGradient_Request :: ~cGradient_Request( void )
{

}

void cGradient_Request :: Draw( void )
{
	Render_Basic();

	// set camera position
	if( !m_no_camera )
	{
		glTranslatef( m_rect.m_x - pActive_Camera->m_x, m_rect.m_y - pActive_Camera->m_y, m_pos_z );
	}
	// ignore camera position
	else
	{
		glTranslatef( m_rect.m_x, m_rect.m_y, m_pos_z );
	}

	if( glIsEnabled( GL_TEXTURE_2D ) )
	{
		glDisable( GL_TEXTURE_2D );
	}

	Render_Advanced();

	if( m_dir == DIR_VERTICAL )
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		GLfloat vertex2[]={0.0f, 0.0f,
                        m_rect.m_w, 0.0f,
                        m_rect.m_w, m_rect.m_h,
                        0.0f, m_rect.m_h};

		Uint8 colorv[]={m_color_1.red, m_color_1.green, m_color_1.blue, m_color_1.alpha,
			m_color_1.red, m_color_1.green, m_color_1.blue, m_color_1.alpha,
			m_color_2.red, m_color_2.green, m_color_2.blue, m_color_2.alpha,
			m_color_2.red, m_color_2.green, m_color_2.blue, m_color_2.alpha};
    
		
		glVertexPointer(2, GL_FLOAT, 0, vertex2);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorv);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}
	else if( m_dir == DIR_HORIZONTAL )
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		GLfloat vertex2[]={0.0f, m_rect.m_h,
                        0.0f, 0.0f,
                        m_rect.m_w, 0.0f,
                        m_rect.m_w, m_rect.m_h};

		Uint8 colorv[]={m_color_1.red, m_color_1.green, m_color_1.blue, m_color_1.alpha,
			m_color_1.red, m_color_1.green, m_color_1.blue, m_color_1.alpha,
			m_color_2.red, m_color_2.green, m_color_2.blue, m_color_2.alpha,
			m_color_2.red, m_color_2.green, m_color_2.blue, m_color_2.alpha};
    
		glVertexPointer(2, GL_FLOAT, 0, vertex2);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorv);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}

	// clear color
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	Render_Advanced_Clear();
	Render_Basic_Clear();

}

/* *** *** *** *** *** *** cCircle_Request *** *** *** *** *** *** *** *** *** *** *** */

cCircle_Request :: cCircle_Request( void )
: cRender_Request_Advanced()
{
	m_type = REND_CIRCLE;
	m_color = static_cast<Uint8>(0);
	m_pos = GL_point( 0.0f, 0.0f );
	m_radius = 0.1f;
	// default is filled
	m_line_width = 0;
}

cCircle_Request :: ~cCircle_Request( void )
{

}

void cCircle_Request :: Draw( void )
{
	Render_Basic();

	// set camera position
	if( !m_no_camera )
	{
		glTranslatef( m_pos.m_x - pActive_Camera->m_x, m_pos.m_y - pActive_Camera->m_y, m_pos_z );
	}
	// ignore camera position
	else
	{
		glTranslatef( m_pos.m_x, m_pos.m_y, m_pos_z );
	}

	Render_Advanced();

	// color
	if( m_color.red != 255 || m_color.green != 255 || m_color.blue != 255 || m_color.alpha != 255 )
	{
		glColor4ub( m_color.red, m_color.green, m_color.blue, m_color.alpha );
	}

	if( glIsEnabled( GL_TEXTURE_2D ) )
	{
		glDisable( GL_TEXTURE_2D );
	}

	// not filled
	if( m_line_width )
	{
		// set line width
		glLineWidth( m_line_width );
	}
    
    glEnableClientState(GL_VERTEX_ARRAY);

    const int VERTICIES=180; // more than needed
    float coords[VERTICIES * 3];
    float theta = 0;
    
    for (int i = 0; i < VERTICIES * 3; i += 3) {
        coords[i + 0] = (float) cos(theta);
        coords[i + 1] = (float) sin(theta);
        coords[i + 2] = 0;
        theta += M_PI / 90;
    }

    glVertexPointer(3, GL_FLOAT, 0, coords);
    glDrawArrays(GL_TRIANGLE_FAN, VERTICIES, GL_FLOAT);

    glDisableClientState(GL_VERTEX_ARRAY);

	// clear line width
	if( m_line_width != 1 )
	{
		glLineWidth( 1.0f );
	}

	// clear color
	if( m_color.red != 255 || m_color.green != 255 || m_color.blue != 255 || m_color.alpha != 255 )
	{
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	Render_Advanced_Clear();
	Render_Basic_Clear();
}

/* *** *** *** *** *** *** cSurface_Request *** *** *** *** *** *** *** *** *** *** *** */

cSurface_Request :: cSurface_Request( void )
: cRender_Request_Advanced()
{
	m_type = REND_SURFACE;
	m_texture_id = 0;

	m_pos_x = 0.0f;
	m_pos_y = 0.0f;

	m_w = 0.0f;
	m_h = 0.0f;

	m_scale_x = 1.0f;
	m_scale_y = 1.0f;
	m_scale_z = 1.0f;

	m_color = static_cast<Uint8>(255);

	m_delete_texture = 0;
}

cSurface_Request :: ~cSurface_Request( void )
{
	if( m_delete_texture && glIsTexture( m_texture_id ) )
	{
		glDeleteTextures( 1, &m_texture_id );
	}
}

void cSurface_Request :: Draw( void )
{
	// draw shadow
	if( m_shadow_pos )
	{
		// shadow position
		m_pos_x += m_shadow_pos;
		m_pos_y += m_shadow_pos;
		m_pos_z -= 0.000001f;

		// save data
		const Color temp_color = m_color;
		const float temp_shadow_pos = m_shadow_pos;
		const GLint temp_combine_type = m_combine_type;
		const float temp_combine_color[3] = { m_combine_color[0], m_combine_color[1], m_combine_color[2] };

		// temporarily unset to prevent endless loop
		m_shadow_pos = 0;
		// shadow as a white texture
		m_color = black;
		// keep m_shadow_color alpha
		m_color.alpha = m_shadow_color.alpha;
		// combine color
		m_combine_type = GL_REPLACE;
		m_combine_color[0] = static_cast<float>(m_shadow_color.red) / 260;
		m_combine_color[1] = static_cast<float>(m_shadow_color.green) / 260;
		m_combine_color[2] = static_cast<float>(m_shadow_color.blue) / 260;

		// draw shadow
		Draw();

		// set back data
		m_shadow_pos = temp_shadow_pos;
		m_color = temp_color;
		m_combine_type = temp_combine_type;
		m_combine_color[0] = temp_combine_color[0];
		m_combine_color[1] = temp_combine_color[1];
		m_combine_color[2] = temp_combine_color[2];
		m_pos_z += 0.000001f;

		// move back to original position
		m_pos_x -= m_shadow_pos;
		m_pos_y -= m_shadow_pos;
	}

	Render_Basic();

	// get half the size
	const float half_w = m_w / 2;
	const float half_h = m_h / 2;
	// position
	float final_pos_x = m_pos_x + ( half_w * m_scale_x );
	float final_pos_y = m_pos_y + ( half_h * m_scale_y );

	// set camera position
	if( !m_no_camera )
	{
		final_pos_x -= pActive_Camera->m_x;
		final_pos_y -= pActive_Camera->m_y;
	}

	glTranslatef( final_pos_x, final_pos_y, m_pos_z );

	// scale
	if( m_scale_x != 1.0f || m_scale_y != 1.0f || m_scale_z != 1.0f )
	{
		glScalef( m_scale_x, m_scale_y, m_scale_z );
	}

	Render_Advanced();

	// color
	if( m_color.red != 255 || m_color.green != 255 || m_color.blue != 255 || m_color.alpha != 255 )
	{
		glColor4ub( m_color.red, m_color.green, m_color.blue, m_color.alpha );
	}

	if( !glIsEnabled( GL_TEXTURE_2D ) )
	{
		glEnable( GL_TEXTURE_2D );
	}

	// only bind if not the same texture
	if( last_bind_texture != m_texture_id )
	{
		glBindTexture( GL_TEXTURE_2D, m_texture_id );
		last_bind_texture = m_texture_id;
	}

    GLfloat vertex2[]={-half_w, -half_h,
                        half_w, -half_h,
                        half_w, half_h,
                        -half_w, half_h};
    GLfloat textureCoor2[]={0.0f, 0.0f,
                            1.0f, 0.0f,
                            1.0f, 1.0f,
                            0.0f, 1.0f};
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 0, vertex2);
    glTexCoordPointer( 2, GL_FLOAT, 0, textureCoor2);
    
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// clear color
	if( m_color.red != 255 || m_color.green != 255 || m_color.blue != 255 || m_color.alpha != 255 )
	{
		/* alpha is automatically 1 for glColor3f
		 * update : not on the shitty intel drivers :(
		*/
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	}

	Render_Advanced_Clear();
	Render_Basic_Clear();
}

/* *** *** *** *** *** *** cRenderQueue *** *** *** *** *** *** *** *** *** *** *** */

cRenderQueue :: cRenderQueue( unsigned int reserve_items )
{
	m_render_data.reserve( reserve_items );
}

cRenderQueue :: ~cRenderQueue( void )
{
	Clear();
}

void cRenderQueue :: Add( cRender_Request *obj )
{
	if( !obj )
	{
		return;
	}

	// if no type
	if( obj->m_type == REND_NOTHING )
	{
		delete obj;
		return;
	}

	m_render_data.push_back( obj );
}

void cRenderQueue :: Render( bool clear /* = 1 */ )
{
	// z position sort
	std::sort( m_render_data.begin(), m_render_data.end(), zpos_sort() );
	// reset last texture
	last_bind_texture = 0;

	for( RenderList::iterator itr = m_render_data.begin(); itr != m_render_data.end(); ++itr )
	{
		cRender_Request *obj = (*itr);

		obj->Draw();
		obj->m_render_count--;
	}

	if( clear )
	{
		Clear( 0 );
	}
}

void cRenderQueue :: Fake_Render( unsigned int amount /* = 1 */, bool clear /* = 1 */ )
{
	for( RenderList::iterator itr = m_render_data.begin(); itr != m_render_data.end(); ++itr )
	{
		cRender_Request *obj = (*itr);
		obj->m_render_count -= amount;
	}

	if( clear )
	{
		Clear( 0 );
	}
}

void cRenderQueue :: Clear( bool force /* = 1 */ )
{
	for( RenderList::iterator itr = m_render_data.begin(); itr != m_render_data.end(); )
	{
		cRender_Request *obj = (*itr);

		// if forced or finished rendering
		if( force || obj->m_render_count <= 0 )
		{
			itr = m_render_data.erase( itr );
			delete obj;
		}
		// increment
		else
		{
			++itr;
		}
	}
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

cRenderQueue *pRenderer = NULL;
cRenderQueue *pRenderer_current = NULL;

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
