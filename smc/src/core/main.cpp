/***************************************************************************
 * main.cpp  -  main routines and initialization
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

#include "../core/global_basic.h"
#include "../core/game_core.h"
#include "../core/main.h"
#include "../core/filesystem/resource_manager.h"
#include "../core/filesystem/filesystem.h"
#include "../level/level.h"
#include "../gui/menu.h"
#include "../core/framerate.h"
#include "../video/font.h"
#include "../user/preferences.h"
#include "../audio/sound_manager.h"
#include "../audio/audio.h"
#include "../level/level_editor.h"
#include "../overworld/world_editor.h"
#include "../overworld/world_manager.h"
#include "../overworld/overworld.h"
#include "../core/campaign_manager.h"
#include "../input/mouse.h"
#include "../user/savegame.h"
#include "../input/keyboard.h"
#include "../video/renderer.h"
#include "../core/i18n.h"
#include "../gui/generic.h"
#include "../input/joystick.h"

// CEGUI
#include "CEGUIDefaultLogger.h"

// SMC namespace is set later to exclude main() from it
using namespace SMC;

// SDLmain defines this for Win32 applications but under debug we use the console
#if defined( __WIN32__ ) && defined( _DEBUG )
	#undef main
#endif

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

int main( int argc, char **argv )
{
	try
	{
		// initialize everything
		Init_Game();
	}
	catch( const std::exception &e )
	{
		printf( "Initialization: Exception raised: %s\n", e.what() );
		return EXIT_FAILURE;
	}

#if 0
	// enter world
 	Game_Action = GA_ENTER_WORLD;
 	Game_Action_Data_Middle.add( "enter_world", "3" );

	// enter level
	Game_Action = GA_ENTER_LEVEL;
 	Game_Mode_Type = MODE_TYPE_LEVEL_CUSTOM;
 	Game_Action_Data_Middle.add( "load_level", arguments[2] );
#endif

	Game_Action = GA_ENTER_MENU;
 	Game_Action_Data_Middle.add( "load_menu", int_to_string( MENU_MAIN ) );

	Game_Action_Data_Start.add( "screen_fadeout", CEGUI::PropertyHelper::intToString( EFFECT_OUT_BLACK ) );
	Game_Action_Data_Start.add( "screen_fadeout_speed", "3" );
	Game_Action_Data_End.add( "screen_fadein", CEGUI::PropertyHelper::intToString( EFFECT_IN_BLACK ) );
	Game_Action_Data_End.add( "screen_fadein_speed", "3" );

	// game loop
	while( !game_exit )
	{
		// update
		Update_Game();
		// draw
		Draw_Game();

		// render
		pVideo->Render();

		// update speedfactor
		pFramerate->Update();
	}

	Exit_Game();
	return EXIT_SUCCESS;
}

// namespace is set here to exclude main() from it
namespace SMC
{

void Init_Game( void )
{
	// init random number generator
	srand( static_cast<unsigned int>(time( NULL )) );

	// Init Stage 1 - core classes
	pResource_Manager = new cResource_Manager();
	pVideo = new cVideo();
	pAudio = new cAudio();
	pFont = new cFont_Manager();
	pFramerate = new cFramerate();
	pRenderer = new cRenderQueue( 200 );
	pRenderer_current = new cRenderQueue( 200 );
	pPreferences = new cPreferences();
	pImage_Manager = new cImage_Manager();
	pSound_Manager = new cSound_Manager();
	pSettingsParser = new cImage_Settings_Parser();

	// Init Stage 2 - set preferences and init audio and the video screen
	/* Set default user directory
	 * can get overridden later from the preferences
	*/
	pResource_Manager->Set_User_Directory( Get_User_Directory() );
	/* Initialize the fake CEGUI renderer and system for the pPreferences XMLParser,
	 * because CEGUI creates the system normally with the OpenGL-Renderer and OpenGL calls may 
	 * only be made with a valid OpenGL-context, which we would get only by setting 
	 * the videomode first. That would mean we need to init the videomode twice.
	 *
	 * The XMLParser can not be used without an initialized CEGUI::System because the XMLParser
	 * uses the CEGUI::System internally. Tested with CEGUI 0.7.4.
	*/
	pVideo->Init_CEGUI_Fake();
	// load user data
	pPreferences->Load();
	// set game language
	I18N_Set_Language( pPreferences->m_language );
	// init translation support
	I18N_Init();
	// delete CEGUI System fake
	pVideo->Delete_CEGUI_Fake();

	// init user dir directory
	pResource_Manager->Init_User_Directory();
	// video init
	pVideo->Init_SDL();
	pVideo->Init_Video();
	pVideo->Init_CEGUI();
	pVideo->Init_CEGUI_Data();
	pFont->Init();
	// framerate init ( must be after SDL init because of SDL_GetTicks() )
	pFramerate->Init();
	// audio init
	pAudio->Init();

	pCampaign_Manager = new cCampaign_Manager();
	pLevel_Player = new cLevel_Player( NULL );
	pLevel_Player->m_disallow_managed_delete = 1;
	// set the first active player available
	pActive_Player = pLevel_Player;
	pLevel_Manager = new cLevel_Manager();
	// set the first animation manager available
	pActive_Animation_Manager = pActive_Level->m_animation_manager;
	// set the first active sprite manager available
	pLevel_Player->Set_Sprite_Manager( pActive_Level->m_sprite_manager );

	// apply preferences
	pPreferences->Apply();

	// draw generic loading screen
	Loading_Screen_Init();

	// Init Stage 3 - game classes
	// note : set any sprite manager as it is set again on game mode switch
	pHud_Manager = new cHud_Manager( pActive_Level->m_sprite_manager );
	pLevel_Player->Init();
	pLevel_Editor = new cEditor_Level( pActive_Level->m_sprite_manager, pActive_Level );
	/* note : set any sprite manager as cOverworld_Manager::Load sets it again 
	 * parent overworld is also set from there again
	*/
	pWorld_Editor = new cEditor_World( pActive_Level->m_sprite_manager, NULL );
	pMouseCursor = new cMouseCursor( pActive_Level->m_sprite_manager );
	pKeyboard = new cKeyboard();

	Joystick::Instance().createJoystick(pActive_Level->m_sprite_manager);

	pLevel_Manager->Init();
	// note : set any sprite manager as cOverworld_Manager::Load sets it again
	pOverworld_Player = new cOverworld_Player( pActive_Level->m_sprite_manager, NULL );
	pOverworld_Manager = new cOverworld_Manager( pActive_Level->m_sprite_manager );
	// set default overworld active
	pOverworld_Player->Set_Overworld( pOverworld_Manager->Get( "World 1" ) );
	pOverworld_Manager->Set_Active( "World 1" );
	pHud_Manager->Load();
	pMenuCore = new cMenuCore();
	pSavegame = new cSavegame();

	// cache
// 	Preload_Images( 1 );
// 	Preload_Sounds( 1 );
	Loading_Screen_Exit();
}

void Exit_Game( void )
{
	if( pPreferences )
	{
		pPreferences->Save();
	}

	pLevel_Manager->Unload();
	pMenuCore->m_handler->m_level->Unload();

	if( pAudio )
	{
		delete pAudio;
		pAudio = NULL;
	}

	if( pLevel_Player )
	{
		delete pLevel_Player;
		pLevel_Player = NULL;
	}

	if( pHud_Manager )
	{
		delete pHud_Manager;
		pHud_Manager = NULL;
	}

	if( pSound_Manager )
	{
		delete pSound_Manager;
		pSound_Manager = NULL;
	}

	if( pLevel_Editor )
	{
		delete pLevel_Editor;
		pLevel_Editor = NULL;
	}

	if( pWorld_Editor )
	{
		delete pWorld_Editor;
		pWorld_Editor = NULL;
	}

	if( pPreferences )
	{
		delete pPreferences;
		pPreferences = NULL;
	}

	if( pSavegame )
	{
		delete pSavegame;
		pSavegame = NULL;
	}

	if( pMouseCursor )
	{
		delete pMouseCursor;
		pMouseCursor = NULL;
	}

	if( pKeyboard )
	{
		delete pKeyboard;
		pKeyboard = NULL;
	}

	if( pCampaign_Manager )
	{
		delete pCampaign_Manager;
		pCampaign_Manager = NULL;
	}
	
	if( pOverworld_Manager )
	{
		delete pOverworld_Manager;
		pOverworld_Manager = NULL;
	}

	if( pOverworld_Player )
	{
		delete pOverworld_Player;
		pOverworld_Player = NULL;
	}

	if( pLevel_Manager )
	{
		delete pLevel_Manager;
		pLevel_Manager = NULL;
	}

	if( pMenuCore )
	{
		delete pMenuCore;
		pMenuCore = NULL;
	}

	if( pRenderer )
	{
		delete pRenderer;
		pRenderer = NULL;
	}

	if( pRenderer_current )
	{
		delete pRenderer_current;
		pRenderer_current = NULL;
	}

	if( pGuiSystem )
	{
		CEGUI::ResourceProvider* rp = pGuiSystem->getResourceProvider();
		CEGUI::Logger *logger = CEGUI::Logger::getSingletonPtr();
		pGuiSystem->destroy();
		pGuiSystem = NULL;
		delete rp;
		delete logger;
	}

	if( pGuiRenderer )
	{
		pGuiRenderer->destroy( *pGuiRenderer );
		pGuiRenderer = NULL;
	}

	if( pVideo )
	{
		delete pVideo;
		pVideo = NULL;
	}

	if( pImage_Manager )
	{
		delete pImage_Manager;
		pImage_Manager = NULL;
	}

	if( pSettingsParser )
	{
		delete pSettingsParser;
		pSettingsParser = NULL;
	}

	if( pFont )
	{
		delete pFont;
		pFont = NULL;
	}

	if( pResource_Manager )
	{
		delete pResource_Manager;
		pResource_Manager = NULL;
	}

	const char *last_sdl_error = SDL_GetError();
	if( strlen( last_sdl_error ) > 0 )
	{
		printf( "Last known SDL Error : %s\n", last_sdl_error );
	}

	// unload the sdl_image preloaded libraries
	IMG_Quit();

	SDL_Quit();
}

bool Handle_Input_Global( SDL_Event *ev )
{
	switch( ev->type )
	{
		case SDL_QUIT:
		{
			game_exit = 1;
			Clear_Input_Events();

			// handle on all handlers ?
			return 0;
		}
		case SDL_VIDEORESIZE:
		{
//			pGuiSystem->notifyDisplaySizeChanged( CEGUI::Size( static_cast<float>(ev->resize.w), static_cast<float>(ev->resize.h) ) );
			break;
		}
		case SDL_KEYDOWN:
		{
			if( pKeyboard->Key_Down( ev->key.keysym.sym ) )
			{
				return 1;
			}
			break;
		}
		case SDL_KEYUP:
		{
			if( pKeyboard->Key_Up( ev->key.keysym.sym ) )
			{
				return 1;
			}
			break;
		}
		case SDL_ACTIVEEVENT:
		{
#if 0
			// lost visibility
			if( ev->active.gain == 0 )
			{
				bool music_paused = 0;
				// pause music
				if( pAudio->Is_Music_Playing() )
				{
					pAudio->Pause_Music();
					music_paused = 1;
				}
				SDL_WaitEvent( NULL );
				// resume if music got paused
				if( music_paused )
				{
					pAudio->Resume_Music();
				}
				return 1;
			}
#endif
			break;
		}
		default: // other events
		{
			// mouse
			if( pMouseCursor->Handle_Event( ev ) )
			{
				return 1; 
			}

			// send events
			if( Game_Mode == MODE_LEVEL )
			{
				// editor events
				if( pLevel_Editor->m_enabled )
				{
					if( pLevel_Editor->Handle_Event( ev ) )
					{
						return 1;
					}
				}
			}
			else if( Game_Mode == MODE_OVERWORLD )
			{
				// editor events
				if( pWorld_Editor->m_enabled )
				{
					if( pWorld_Editor->Handle_Event( ev ) )
					{
						return 1;
					}
				}
			}
			else if( Game_Mode == MODE_MENU )
			{
				if( pMenuCore->Handle_Event( ev ) )
				{
					return 1;
				}
			}
			break;
		}
	}

	return 0;
}

void Update_Game( void )
{
	// do not update if exiting
	if( game_exit )
	{
		return;
	}

	// if in menu and vsync is disabled then limit the fps to reduce the load for CPU/GPU
	if( Game_Mode == MODE_MENU)
	{
		Correct_Frame_Time( 100 );
	}
	// if fps limit is set
	else
	{
		Correct_Frame_Time( 60 );
	}
	
	// ## game events
	Handle_Game_Events();

	// ## input
	while( SDL_PollEvent( &input_event ) )
	{
		// handle
		Handle_Input_Global( &input_event );
	}

	pMouseCursor->Update();

	// ## audio
	pAudio->Resume_Music();
	pAudio->Update();

	// performance measuring
	pFramerate->m_perf_last_ticks = SDL_GetTicks();

	// ## update
	if( Game_Mode == MODE_LEVEL )
	{
		pLevel_Manager->Update();
	}
	else if( Game_Mode == MODE_OVERWORLD )
	{
		pActive_Overworld->Update();
	}
	else if( Game_Mode == MODE_MENU )
	{
		pMenuCore->Update();
	}
	else if( Game_Mode == MODE_LEVEL_SETTINGS )
	{
		pLevel_Editor->m_settings_screen->Update();
	}

	// gui
	Gui_Handle_Time();
}

void Draw_Game( void )
{
	// don't draw if exiting
	if( game_exit )
	{
		return;
	}

	// performance measuring
	pFramerate->m_perf_last_ticks = SDL_GetTicks();

	if( Game_Mode == MODE_LEVEL )
	{
		pLevel_Manager->Draw();
	}
	else if( Game_Mode == MODE_OVERWORLD )
	{
		pActive_Overworld->Draw();
	}
	else if( Game_Mode == MODE_MENU )
	{
		pMenuCore->Draw();
	}
	else if( Game_Mode == MODE_LEVEL_SETTINGS )
	{
		pLevel_Editor->m_settings_screen->Draw();
	}

	Joystick::Instance().show();
	// Mouse
	pMouseCursor->Draw();

	// update performance timer
	pFramerate->m_perf_timer[PERF_DRAW_MOUSE]->Update();
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
