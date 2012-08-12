/***************************************************************************
 * world_player.cpp  - Overworld Player class
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
 
#include "../overworld/world_player.h"
#include "../core/game_core.h"
#include "../core/framerate.h"
#include "../overworld/overworld.h"
#include "../level/level.h"
#include "../video/font.h"
#include "../audio/audio.h"
#include "../gui/menu.h"
#include "../video/renderer.h"
#include "../user/savegame.h"

namespace SMC
{

/* *** *** *** *** *** *** *** *** cOverworld_Player *** *** *** *** *** *** *** *** *** */

cOverworld_Player :: cOverworld_Player( cSprite_Manager *sprite_manager, cOverworld *overworld )
: cAnimated_Sprite( sprite_manager )
{
	m_sprite_array = ARRAY_PLAYER;
	m_type = TYPE_PLAYER;
	m_pos_z = 0.0999f;
	m_massive_type = MASS_MASSIVE;
	m_camera_range = 0;
	m_name = "Maryo";

	m_overworld = overworld;
	m_current_waypoint = -2; // no waypoint
	m_line_waypoint = 0;
	m_current_line = -2;

	m_fixed_walking = 0;

	m_line_hor = cLine_collision();
	m_line_ver = cLine_collision();

	m_debug_current_line_last = -100;
	m_debug_lines_last = -100;
	m_debug_current_waypoint_last = -100;

	m_debug_lines = new cHudSprite( m_sprite_manager );
	m_debug_lines->Set_Pos( 150, game_res_h * 0.97f, 1 );
	m_debug_current_line = new cHudSprite( m_sprite_manager );
	m_debug_current_line->Set_Pos( 300, game_res_h * 0.97f, 1 );
	m_debug_current_waypoint = new cHudSprite( m_sprite_manager );
	m_debug_current_waypoint->Set_Pos( 500, game_res_h * 0.97f, 1 );

	Set_Type( MARYO_SMALL );
	m_direction = DIR_DOWN;
	Set_Direction( DIR_UNDEFINED );
}

cOverworld_Player :: ~cOverworld_Player( void )
{
	delete m_debug_lines;
	delete m_debug_current_line;
	delete m_debug_current_waypoint;

	Unload_Images();
}

void cOverworld_Player :: Load_Images( void )
{
	Unload_Images();

	if( m_maryo_state == MARYO_SMALL )
	{
		Add_Image( pVideo->Get_Surface( "world/maryo/small/down.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/down_1.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/down.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/down_2.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/up_1.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/up.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/up_1.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/up_2.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/left.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/left_1.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/left.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/left_2.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/right.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/right_1.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/right.png" ) );
		Add_Image( pVideo->Get_Surface( "world/maryo/small/right_2.png" ) );

		Set_Animation( 1 );
	}
	else
	{
		printf( "Unsupported Maryo state : %d\n", static_cast<unsigned int>(m_maryo_state) );
		return;
	}
	
	Set_Image_Num( 0, 1 );	
}

void cOverworld_Player :: Unload_Images( void )
{
	Clear_Images();
	Reset_Animation();
}

void cOverworld_Player :: Set_Overworld( cOverworld *overworld )
{
	m_overworld = overworld;
}

void cOverworld_Player :: Set_Direction( const ObjectDirection dir, bool new_start_direction /* = 0 */ )
{
	if( dir != m_direction )
	{
		Reset_Animation();

		// undefined is set if not moving
		if( dir == DIR_UNDEFINED )
		{
			Set_Animation_Image_Range( 0, 3 );
			Set_Time_All( 300, 1 );
		}
		else if( dir == DIR_DOWN )
		{
			Set_Animation_Image_Range( 0, 3 );
			Set_Time_All( 200, 1 );
		}
		else if( dir == DIR_UP )
		{
			Set_Animation_Image_Range( 4, 7 );
			Set_Time_All( 200, 1 );
		}
		else if( dir == DIR_LEFT )
		{
			Set_Animation_Image_Range( 8, 11 );
			Set_Time_All( 200, 1 );
		}
		else if( dir == DIR_RIGHT )
		{
			Set_Animation_Image_Range( 12, 15 );
			Set_Time_All( 200, 1 );
		}

		Set_Image_Num( m_anim_img_start );
	}

	cAnimated_Sprite::Set_Direction( dir, new_start_direction );

	Update_Vel();
}

void cOverworld_Player :: Set_Type( Maryo_type new_type )
{
	// already set
	if( m_maryo_state == new_type )
	{
		return;
	}

	m_maryo_state = new_type;
	Load_Images();
}

void cOverworld_Player :: Update( void )
{
	cAnimated_Sprite::Update();

	Update_Animation();

	if( m_direction == DIR_UNDEFINED )
	{
		return;
	}

	// default walking
	if( !m_fixed_walking )
	{
		Update_Walk();
	} 
	// fixed walking
	else
	{
		Update_Waypoint_Walk();
	}
}

void cOverworld_Player :: Draw( cSurface_Request *request /* = NULL */ )
{
	bool create_request = 0;

	if( !request )
	{
		create_request = 1;
		// create request
		request = new cSurface_Request();
	}

	// Draw player
	cAnimated_Sprite::Draw( request );
	// alpha in debug mode
	if( pOverworld_Manager->m_debug_mode )
	{
		request->m_color.alpha = 64;
	}

	if( create_request )
	{
		// add request
		pRenderer->Add( request );
	}

	// Draw debug text
	Draw_Debug_Text();
}

void cOverworld_Player :: Draw_Debug_Text( void )
{
	if( !pOverworld_Manager->m_debug_mode )
	{
		return;
	}

	// Update Lines Texts
	if( static_cast<int>(m_overworld->m_layer->size()) != m_debug_lines_last )
	{
		m_debug_lines->Set_Image( pFont->Render_Text( pFont->m_font_small, "Lines : " + int_to_string( m_overworld->m_layer->size() ), blue ), 1, 1 );
	}
	// Update Current Line Texts
	if( m_current_line != m_debug_current_line_last )
	{
		m_debug_current_line->Set_Image( pFont->Render_Text( pFont->m_font_small, "Curr Line : " + int_to_string( m_current_line ), green ), 1, 1 );
	}
	// Update Current Waypoint Texts
	if( m_current_waypoint != m_debug_current_waypoint_last )
	{
		m_debug_current_waypoint->Set_Image( pFont->Render_Text( pFont->m_font_small, "Curr Waypoint : " + int_to_string( m_current_waypoint ), green ), 1, 1 );
	}

	// Draw Line count
	cSurface_Request *request = new cSurface_Request();
	m_debug_lines->Draw( request );
	request->m_shadow_pos = 1.0f;
	request->m_shadow_color = black;
	// add request
	pRenderer->Add( request );

	// Draw Current Line
	request = new cSurface_Request();
	m_debug_current_line->Draw( request );
	request->m_shadow_pos = 1.0f;
	request->m_shadow_color = black;
	// add request
	pRenderer->Add( request );
	
	// Draw Current Waypoint
	request = new cSurface_Request();
	m_debug_current_waypoint->Draw( request );
	request->m_shadow_pos = 1.0f;
	request->m_shadow_color = black;
	// add request
	pRenderer->Add( request );
}

void cOverworld_Player :: Reset( void )
{
	m_current_waypoint = -2;
	m_line_waypoint = 0;
	m_current_line = -2;

	m_fixed_walking = 0;
	Set_Direction( DIR_UNDEFINED );
}

void cOverworld_Player :: Action_Interact( input_identifier key_type )
{
	// Left
	if( key_type == INP_LEFT )
	{
		pOverworld_Player->Start_Walk( DIR_LEFT );
	}
	// Right
	else if( key_type == INP_RIGHT )
	{
		pOverworld_Player->Start_Walk( DIR_RIGHT );
	}
	// Up
	else if( key_type == INP_UP )
	{
		pOverworld_Player->Start_Walk( DIR_UP );
	}
	// Down
	else if( key_type == INP_DOWN )
	{
		pOverworld_Player->Start_Walk( DIR_DOWN );
	}
	// Action
	else if( key_type == INP_ACTION )
	{
		// enter
		pOverworld_Player->Activate_Waypoint();
		Clear_Input_Events();
	}
	// Exit
	else if( key_type == INP_EXIT )
	{
		Game_Action = GA_ENTER_MENU;
		Game_Action_Data_Middle.add( "load_menu", int_to_string( MENU_MAIN ) );
		Game_Action_Data_Middle.add( "menu_exit_back_to", int_to_string( MODE_OVERWORLD ) );
	}
}

void cOverworld_Player :: Action_Stop_Interact( input_identifier key_type )
{
	// nothing yet
}

void cOverworld_Player :: Activate_Waypoint( void )
{
	// if no waypoint or already walking
	if( m_current_waypoint < 0 || m_direction != DIR_UNDEFINED )
	{
		return;
	}

	// get waypoint
	cWaypoint *waypoint = Get_Waypoint();

	// normal level waypoint
	if( waypoint->m_waypoint_type == WAYPOINT_NORMAL )
	{
		// Enter Level
		Game_Action = GA_ENTER_LEVEL;
		Game_Action_Data_Start.add( "music_fadeout", "1000" );
		Game_Action_Data_Start.add( "screen_fadeout", CEGUI::PropertyHelper::intToString( EFFECT_OUT_FIXED_COLORBOX ) );
		Game_Action_Data_Middle.add( "load_level", waypoint->Get_Destination() );
		Game_Action_Data_End.add( "screen_fadein", CEGUI::PropertyHelper::intToString( EFFECT_IN_RANDOM ) );
	}
	// world link waypoint
	else if( waypoint->m_waypoint_type == WAYPOINT_WORLD_LINK )
	{
		// remember Overworld origin
		cOverworld *overworld_origin = m_overworld;

		// world string
		std::string str_world = waypoint->Get_Destination();

		// Enter Credits Menu ( World End )
		if( str_world.compare( "credits" ) == 0 )
		{
			Game_Action = GA_ENTER_MENU;
			Game_Action_Data_Start.add( "music_fadeout", "1500" );
			Game_Action_Data_Start.add( "screen_fadeout", CEGUI::PropertyHelper::intToString( EFFECT_OUT_HORIZONTAL_VERTICAL ) );
			Game_Action_Data_Middle.add( "load_menu", int_to_string( MENU_CREDITS ) );
			Game_Action_Data_Middle.add( "menu_exit_back_to", int_to_string( MODE_OVERWORLD ) );
			Game_Action_Data_End.add( "screen_fadein", CEGUI::PropertyHelper::intToString( EFFECT_IN_RANDOM ) );
		}
		// world link
		else
		{
			if( pOverworld_Manager->Get( str_world ) )
			{
				Game_Action = GA_ENTER_WORLD;
				Game_Action_Data_Start.add( "music_fadeout", "1500" );
				Game_Action_Data_Start.add( "screen_fadeout", CEGUI::PropertyHelper::intToString( EFFECT_OUT_BLACK_TILED_RECTS ) );
				Game_Action_Data_Middle.add( "enter_world", str_world.c_str() );
				Game_Action_Data_Middle.add( "world_player_waypoint", overworld_origin->m_description->m_path.c_str() );
				Game_Action_Data_End.add( "screen_fadein", CEGUI::PropertyHelper::intToString( EFFECT_IN_BLACK ) );
			}
			else
			{
				printf( "Warning : Overworld not found %s\n", str_world.c_str() );
			}
		}
	}
}

void cOverworld_Player :: Update_Vel( void )
{
	if( m_direction == DIR_UP )
	{
		m_velx = 0.0f;
		m_vely = -3.0f;
	}
	else if( m_direction == DIR_DOWN )
	{
		m_velx = 0.0f;
		m_vely = 3.0f;
	}
	else if( m_direction == DIR_LEFT )
	{
		m_velx = -3.0f;
		m_vely = 0.0f;
	}
	else if( m_direction == DIR_RIGHT )
	{
		m_velx = 3.0f;
		m_vely = 0.0f;
	}
}

bool cOverworld_Player :: Start_Walk( ObjectDirection new_direction )
{
	// already walking into the given direction
	if( new_direction == m_direction )
	{
		return 0;
	}

	// invalid direction
	if( !( new_direction == DIR_UP || new_direction == DIR_DOWN || new_direction == DIR_LEFT || new_direction == DIR_RIGHT ) )
	{
		printf( "Warning : New direction is invalid : %d\n", new_direction );
		return 0;
	}

	// print debug info
	if( pOverworld_Manager->m_debug_mode )
	{
		printf( "Current Maryo Direction : %d\n", m_direction );

		if( m_current_waypoint > 0 )
		{
			printf( "Waypoint Direction Forward : %d Backward : %d\n", Get_Waypoint()->m_direction_forward, Get_Waypoint()->m_direction_backward );
		}
	}

	// a start from waypoint
	if( m_current_waypoint >= 0 && m_direction == DIR_UNDEFINED )
	{
		// Get Layer Line in front
		cLayer_Line_Point_Start *front_line = Get_Front_Line( new_direction );

		if( !front_line )
		{
			if( pOverworld_Manager->m_debug_mode )
			{
				printf( "Waypoint Front line not detected\n" );
			}

			return 0;
		}

		if( pOverworld_Manager->m_debug_mode )
		{
			printf( "Waypoint Front line id %d, origin id %d\n", m_overworld->m_layer->Get_Array_Num( front_line ), front_line->m_origin );
		}

		// forward
		if( Get_Waypoint()->m_direction_forward == new_direction )
		{
			cWaypoint *next_waypoint = front_line->Get_End_Waypoint();

			if( !next_waypoint )
			{
				printf( "Next waypoint not detected\n" );
				return 0;
			}

			// next waypoint is not accessible
			if( !next_waypoint->m_access )
			{
				if( pOverworld_Manager->m_debug_mode )
				{
					printf( "No access to next waypoint\n" );
				}

				return 0;
			}

			if( pOverworld_Manager->m_debug_mode )
			{
				printf( "Next waypoint id : %d\n", m_current_waypoint );
			}

			// set line waypoint
			m_line_waypoint = front_line->m_origin;
		}
		// backward
		else if( Get_Waypoint()->m_direction_backward == new_direction )
		{
			// set line waypoint
			m_line_waypoint = front_line->m_origin;
		}
		// invalid direction
		else
		{
			return 0;
		}
	}

	// remember current direction
	ObjectDirection direction_old = m_direction;

	// if already walking
	if( m_direction != DIR_UNDEFINED )
	{
		if( m_direction == Get_Opposite_Direction( new_direction ) && !m_fixed_walking && m_overworld->Get_Waypoint_Collision( m_col_rect ) < 0 )
		{
			Set_Direction( new_direction );
		}
	}
	// start walking
	else
	{
		Set_Direction( new_direction );
		// force a small move into the new direction
		Move( m_velx, m_vely, 1 );
	}

	// if maryo is not on his current waypoint collide with every waypoint
	if( m_current_waypoint >= 0 && !m_col_rect.Intersects( Get_Waypoint()->m_rect ) )
	{
		m_current_waypoint = -1;
	}

	// if maryo is walking into a new direction
	if( direction_old != m_direction )
	{
		return 1;
	}

	// direction unchanged
	return 0;
}

void cOverworld_Player :: Update_Walk( void )
{
	Move( m_velx, m_vely );
	Update_Path_Diff();

	/* Check if Maryo is too far away from the line in the current direction
	 * or if no line is anymore connected
	 * if true change direction
	*/
	if( ( ( m_direction == DIR_UP || m_direction == DIR_DOWN ) && m_line_hor.m_line_number >= 0 && ( m_line_hor.m_difference > 20 || m_line_hor.m_difference < -20 ) ) || 
		( ( m_direction == DIR_LEFT || m_direction == DIR_RIGHT ) && m_line_ver.m_line_number >= 0 && ( m_line_ver.m_difference > 20 || m_line_ver.m_difference < -20 ) ) ||
		 ( m_line_hor.m_line_number < 0 && m_line_ver.m_line_number < 0 ) )
	{
		if( pOverworld_Manager->m_debug_mode )
		{
			if( m_direction == DIR_UP || m_direction == DIR_DOWN )
			{
				printf( "horizontal line connection broken : num %d diff %f\n", m_line_hor.m_line_number, m_line_hor.m_difference );
			}
			else if( m_direction == DIR_LEFT || m_direction == DIR_RIGHT )
			{
				printf( "vertical line connection broken : num %d diff %f\n", m_line_ver.m_line_number, m_line_ver.m_difference );
			}
		}

		m_current_waypoint = -1;

		Change_Direction();
	}

	Auto_Pos_Correction();

	// check if a new waypoint is near maryo
	for( unsigned int i = 0; i < m_overworld->m_waypoints.size(); i++ )
	{
		// skip the start waypoint
		if( static_cast<int>(i) == m_current_waypoint )
		{
			continue;
		}

		if( m_overworld->m_waypoints[i]->m_rect.Intersects( m_col_rect ) )
		{
			Start_Waypoint_Walk( i );
			break;
		}
	}
}

void cOverworld_Player :: Start_Waypoint_Walk( int new_waypoint )
{
	m_fixed_walking = 1;
	m_current_waypoint = new_waypoint;
	m_current_line = -2;

	m_overworld->Update_Waypoint_text();
}

void cOverworld_Player :: Update_Waypoint_Walk( void )
{
	// invalid current waypoint
	if( m_current_waypoint < 0 )
	{
		return;
	}

	unsigned int reached = 0;
	cWaypoint *waypoint = Get_Waypoint();

	// horizontal position to waypoint
	const float x = m_col_rect.m_x + ( m_col_rect.m_w * 0.5f );

	if( waypoint->m_rect.m_x + ( waypoint->m_rect.m_w * 0.4f ) > x )
	{
		Move( 3.0f, 0.0f );
	}
	else if( waypoint->m_rect.m_x + ( waypoint->m_rect.m_w * 0.7f ) < x )
	{
		Move( -3.0f, 0.0f );
	}
	else
	{
		reached++;
	}

	// vertical position to waypoint
	const float y = m_col_rect.m_y + ( m_col_rect.m_h * 0.5f );

	if( waypoint->m_rect.m_y + ( waypoint->m_rect.m_h * 0.4f ) > y )
	{
		Move( 0.0f, 3.0f );
	}
	else if( waypoint->m_rect.m_y + ( waypoint->m_rect.m_h * 0.7f ) < y )
	{
		Move( 0.0f, -3.0f );
	}
	else
	{
		reached++;
	}

	if( reached == 2 )
	{
		m_fixed_walking = 0;

		Set_Direction( DIR_UNDEFINED );
		Set_Waypoint( m_current_waypoint );

		pAudio->Play_Sound( "waypoint_reached.ogg" );

		// ������Ŀ�ĵأ��浵
		pSavegame->Save_Game(0, "AutoSave");
	}
}

bool cOverworld_Player :: Set_Waypoint( int waypoint, bool new_startpos /* = 0 */ )
{
	if( waypoint < 0 || waypoint >= static_cast<int>(m_overworld->m_waypoints.size()) ) 
	{
		return 0;
	}

	cWaypoint *wp = m_overworld->m_waypoints[waypoint];

	Set_Pos( wp->m_rect.m_x - m_col_pos.m_x + ( ( wp->m_rect.m_w - m_col_rect.m_w ) * 0.5f ), wp->m_rect.m_y - m_col_pos.m_y + ( ( wp->m_rect.m_h - m_col_rect.m_h ) * 0.5f ), new_startpos );
	m_current_waypoint = waypoint;

	// Update Camera
	pActive_Camera->Update();
	// Update Waypoint text
	m_overworld->Update_Waypoint_text();

	return 1;
}

cWaypoint *cOverworld_Player :: Get_Waypoint( void )
{
	if( m_current_waypoint < 0 )
	{
		return NULL;
	}

	return m_overworld->Get_Waypoint( m_current_waypoint );
}

cLayer_Line_Point_Start *cOverworld_Player :: Get_Front_Line( ObjectDirection dir ) const
{
	return m_overworld->m_layer->Get_Line_Collision_Direction( m_col_rect.m_x + ( m_col_rect.m_w * 0.5f ), m_col_rect.m_y + ( m_col_rect.m_h * 0.5f ), dir ).m_line;
}

void cOverworld_Player :: Update_Path_Diff( unsigned int check_size /* = 25 */ )
{
	float x = m_col_rect.m_x + ( m_col_rect.m_w * 0.5f ) + m_velx;
	float y = m_col_rect.m_y + ( m_col_rect.m_h * 0.5f ) + m_vely;

	// bigger direction check size
	unsigned int hor_advance = 0;
	unsigned int ver_advance = 0;
	if( m_direction == DIR_LEFT || m_direction == DIR_RIGHT )
	{
		ver_advance += 10;
	}
	else if( m_direction == DIR_UP || m_direction == DIR_DOWN )
	{
		hor_advance += 10;
	}

	// get nearest lines
	m_line_hor = m_overworld->m_layer->Get_Nearest( x, y, DIR_HORIZONTAL, check_size + hor_advance, m_line_waypoint );
 	m_line_ver = m_overworld->m_layer->Get_Nearest( x, y, DIR_VERTICAL, check_size + ver_advance, m_line_waypoint );
}

void cOverworld_Player :: Change_Direction( void )
{
	m_line_hor = cLine_collision();
	m_line_ver = cLine_collision();

	float x = m_col_rect.m_x + ( m_col_rect.m_w * 0.5f );
	float y = m_col_rect.m_y + ( m_col_rect.m_h * 0.5f );

	/* check with a big collision line the difference to the next layer line behind maryo
	 * if no line is found turn around
	 * if a line is found walk into it's direction
	*/
	if( m_direction == DIR_RIGHT || m_direction == DIR_LEFT )
	{
		// search for a line with increasing distance
		for( unsigned int i = 0; i < 30; i++ )
		{
			m_line_ver = m_overworld->m_layer->Get_Nearest( x - i, y, DIR_VERTICAL, 80, m_line_waypoint );

			// found
			if( m_line_ver.m_line_number >= 0 )
			{
				break;
			}

			m_line_ver = m_overworld->m_layer->Get_Nearest( x + i, y, DIR_VERTICAL, 80, m_line_waypoint );

			// found
			if( m_line_ver.m_line_number >= 0 )
			{
				break;
			}
		}

		// if line is found
		if( m_line_ver.m_line_number >= 0 )
		{
			if( m_line_ver.m_difference < 0 )
			{
				Set_Direction( DIR_UP );
			}
			else if( m_line_ver.m_difference > 0 )
			{
				Set_Direction( DIR_DOWN );
			}
		}
		// if no line found walk in to the opposite direction
		else
		{
			Set_Direction( Get_Opposite_Direction( m_direction ) );
		}
	}
	else if( m_direction == DIR_UP || m_direction == DIR_DOWN )
	{
		// search for a line with increasing distance
		for( unsigned int i = 0; i < 30; i++ )
		{
			m_line_hor = m_overworld->m_layer->Get_Nearest( x, y + i, DIR_HORIZONTAL, 80, m_line_waypoint );

			// found
			if( m_line_hor.m_line_number >= 0 )
			{
				break;
			}

			m_line_hor = m_overworld->m_layer->Get_Nearest( x, y - i, DIR_HORIZONTAL, 80, m_line_waypoint );

			// found
			if( m_line_hor.m_line_number >= 0 )
			{
				break;
			}
		}

		// if line is found
		if( m_line_hor.m_line_number >= 0 )
		{
			if( m_line_hor.m_difference < 0 )
			{
				Set_Direction( DIR_LEFT );
			}
			else if( m_line_hor.m_difference > 0 )
			{
				Set_Direction( DIR_RIGHT );
			}
		}
		// if no line found walk in to the opposite direction
		else
		{
			Set_Direction( Get_Opposite_Direction( m_direction ) );
		}
	}
	else
	{
		printf( "Warning : Unknown Maryo direction %d\n", static_cast<int>(m_direction) );
		Set_Direction( DIR_DOWN );
	}

	if( pOverworld_Manager->m_debug_mode )
	{
		printf( "Changed direction to %s\n", Get_Direction_Name( m_direction ).c_str() );
	}
}

void cOverworld_Player :: Auto_Pos_Correction( float size /* = 1.7f */, float min_distance /* = 5.0f */ )
{
	// if colliding with a waypoint
	if( m_overworld->Get_Waypoint_Collision( m_col_rect ) != -1 )
	{
		return;
	}

	if( m_direction == DIR_LEFT || m_direction == DIR_RIGHT )
	{
		// up and down correction
		if( m_line_ver.m_difference > min_distance )
		{
			Move( 0.0f, size );
		}
		else if( m_line_ver.m_difference < -min_distance )
		{
			Move( 0.0f, -size );
		}

		// Set current line number
		if( m_current_line != m_line_ver.m_line_number )
		{
			m_current_line = m_line_ver.m_line_number;
		}
	}
	else if( m_direction == DIR_UP || m_direction == DIR_DOWN )
	{ 
		// left correction
		if( m_line_hor.m_difference < -min_distance )
		{
			Move( -size, 0.0f );
		}
		// right correction
		else if( m_line_hor.m_difference > min_distance )
		{
			Move( size, 0.0f );
		}

		// Set current line number
		if( m_current_line != m_line_hor.m_line_number )
		{
			m_current_line = m_line_hor.m_line_number;
		}
	}
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

cOverworld_Player *pOverworld_Player = NULL;

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
