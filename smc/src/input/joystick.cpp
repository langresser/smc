#include "joystick.h"
#include "../core/sprite_manager.h"
#include "../objects/sprite.h"
#include "../video/font.h"
#include "../video/color.h"
#include "../core/game_core.h"
#include "../overworld/world_player.h"
#include "../level/level_player.h"
#include "../gui/menu.h"

namespace SMC {

void Joystick::createJoystick(cSprite_Manager *sprite_manager)
{
//	m_dir = DIR_NONE;
//    m_menuDown = false;
//    m_jumpDown = false;
//    m_fireDown = false;
//    m_showMenuBack = false;
//
//	m_spriteManager = sprite_manager;
//
//	m_dock = new cHudSprite(sprite_manager);
//	m_dock->Set_Image(pVideo->Get_Surface("extern/jsbg.png"));
//	m_dock->Set_Pos( 5, 320, 1 );
//
//	m_up = new cHudSprite(sprite_manager);
//	m_up->Set_Image(pVideo->Get_Surface("extern/jsup.png"));
//	m_up->Set_Pos( 5, 290, 1 );
//
//	m_down = new cHudSprite(sprite_manager);
//	m_down->Set_Image(pVideo->Get_Surface("extern/jsdown.png"));
//	m_down->Set_Pos( 5, 320, 1 );
//
//	m_left = new cHudSprite(sprite_manager);
//	m_left->Set_Image(pVideo->Get_Surface("extern/jsleft.png"));
//	m_left->Set_Pos( 5, 150, 1 );
//
//	m_right = new cHudSprite(sprite_manager);
//	m_right->Set_Image(pVideo->Get_Surface("extern/jsright.png"));
//	m_right->Set_Pos( 5, 150, 1 );
//
//	m_menu = new cHudSprite(sprite_manager);
//	m_menu->Set_Image(pVideo->Get_Surface("extern/menunormal.png"));
//	m_menu->Set_Pos(2, 2, 1);
//
//	m_menuClick = new cHudSprite(sprite_manager);
//	m_menuClick->Set_Image(pVideo->Get_Surface("extern/menuclick.png"));
//	m_menuClick->Set_Pos(2, 2, 1);
//    
//    m_menuBack = new cHudSprite(sprite_manager);
//	m_menuBack->Set_Image(pVideo->Get_Surface("extern/back1.png"));
//	m_menuBack->Set_Pos(2, 2, 1);
//    
//	m_menuBackClick = new cHudSprite(sprite_manager);
//	m_menuBackClick->Set_Image(pVideo->Get_Surface("extern/back.png"));
//	m_menuBackClick->Set_Pos(2, 2, 1);
//
//	m_jump = new cHudSprite(sprite_manager);
//	m_jump->Set_Image(pVideo->Get_Surface("extern/anormal.png"));
//
//	m_jumpClick = new cHudSprite(sprite_manager);
//	m_jumpClick->Set_Image(pVideo->Get_Surface("extern/aclick.png"));
//
//	m_fire = new cHudSprite(sprite_manager);
//	m_fire->Set_Image(pVideo->Get_Surface("extern/bnormal.png"));
//    
//	m_fireClick = new cHudSprite(sprite_manager);
//	m_fireClick->Set_Image(pVideo->Get_Surface("extern/bclick.png"));
//
//    int jumpx = game_res_w - m_fire->m_col_rect.m_w - m_jump->m_col_rect.m_w - 20;
//    int jumpy = game_res_h - m_jump->m_col_rect.m_h - 10;
//
//    int firex = game_res_w - m_fire->m_col_rect.m_w - 10;
//    int firey = game_res_h - m_jump->m_col_rect.m_h - m_fire->m_col_rect.m_h - 20;
//    
//    m_jump->Set_Pos(jumpx, jumpy, 1);
//	m_jumpClick->Set_Pos(jumpx, jumpy, 1);
//	m_fire->Set_Pos(firex, firey, 1);
//	m_fireClick->Set_Pos(firex, firey, 1);
}
    
bool Joystick::isTouchIn(int touchx, int touchy, int x, int y, int w, int h)
{
    if (touchx >= x && touchx <= x + w
        && touchy >= y && touchy <= y + h) {
        return true;
    }
    
    return false;
}
    
void Joystick::touchEvent(int type, int x, int y, int figureId)
{
    if (!m_showMenuBack && Game_Mode != MODE_LEVEL && Game_Mode != MODE_OVERWORLD) {
        return;
    }
    return;
    
    switch (type) {
        case SDL_MOUSEBUTTONDOWN:
        {
            int width = m_menu->m_rect.m_w;
            if (isTouchIn(x, y, 0, 0, width, width)) {
                m_menuDown = true;
            }
        }
            break;
        case SDL_MOUSEBUTTONUP:
        {
            int width = m_menu->m_rect.m_w;
            if (m_menuDown && isTouchIn(x, y, 0, 0, width, width)) {
                if (m_showMenuBack) {
                    m_showMenuBack = false;
                    pMenuCore->Key_Down(SDLK_ESCAPE);
                } else {
                    m_showMenuBack = true;
                    if (Game_Mode == MODE_OVERWORLD) {
                        pOverworld_Player->Action_Interact( INP_EXIT );
                    } else {
                        pLevel_Player->Action_Interact( INP_EXIT );
                    }
                }
                
            }
            m_menuDown = false;
        }
            break;
        case SDL_MOUSEMOTION:
            break;
        default:
            break;
    }
}

void Joystick::show()
{
    return;
    if (m_showMenuBack) {
        if (m_menuDown) {
            m_menuBackClick->Draw();
        } else {
            m_menuBack->Draw();
        }
        return;
    }

    if (Game_Mode != MODE_LEVEL && Game_Mode != MODE_OVERWORLD) {
        return;
    }
	m_dock->Draw( NULL );
	m_dir = DIR_UP;
	switch (m_dir)
	{
	case DIR_UP:
		m_up->Draw(NULL);
		break;
	case DIR_DOWN:
		m_down->Draw(NULL);
		break;
	case DIR_LEFT:
		m_left->Draw(NULL);
		break;
	case DIR_RIGHT:
		m_right->Draw(NULL);
		break;
	}

    if (m_menuDown) {
        m_menuClick->Draw();
    } else {
        m_menu->Draw(NULL);
    }
	
    if (m_jumpDown) {
        m_jumpClick->Draw(NULL);
    } else {
        m_jump->Draw(NULL);
    }
	
    if (m_fireDown) {
        m_fireClick->Draw(NULL);
    } else {
        m_fire->Draw(NULL);
    }
}

Joystick& Joystick::Instance()
{
	static Joystick s_joystick;
	return s_joystick;
}

}