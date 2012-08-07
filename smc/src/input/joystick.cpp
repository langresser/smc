#include "joystick.h"
#include "../core/sprite_manager.h"
#include "../objects/sprite.h"
#include "../video/font.h"
#include "../video/color.h"

namespace SMC {

void Joystick::createJoystick(cSprite_Manager *sprite_manager)
{
	m_dir = DIR_NONE;
	m_spriteManager = sprite_manager;

	m_dock = new cHudSprite(sprite_manager);
	m_dock->Set_Image(pVideo->Get_Surface("extern/jsbg.png"));
	m_dock->Set_Pos( 20, 150, 1 );

	m_up = new cHudSprite(sprite_manager);
	m_up->Set_Image(pVideo->Get_Surface("extern/jsup.png"));
	m_up->Set_Pos( 20, 150, 1 );

	m_down = new cHudSprite(sprite_manager);
	m_down->Set_Image(pVideo->Get_Surface("extern/jsdown.png"));
	m_down->Set_Pos( 20, 150, 1 );

	m_left = new cHudSprite(sprite_manager);
	m_left->Set_Image(pVideo->Get_Surface("extern/jsleft.png"));
	m_left->Set_Pos( 20, 150, 1 );

	m_right = new cHudSprite(sprite_manager);
	m_right->Set_Image(pVideo->Get_Surface("extern/jsright.png"));
	m_right->Set_Pos( 20, 150, 1 );

	m_menu = new cHudSprite(sprite_manager);
	m_menu->Set_Image(pVideo->Get_Surface("extern/menunormal.png"));
	m_menu->Set_Pos(2, 2, 1);

	m_menuClick = new cHudSprite(sprite_manager);
	m_menuClick->Set_Image(pVideo->Get_Surface("extern/menuclick.png"));
	m_menuClick->Set_Pos(2, 2, 1);

	m_jump = new cHudSprite(sprite_manager);
	m_jump->Set_Image(pVideo->Get_Surface("extern/back1.png"));
	m_jump->Set_Pos(430, 250, 1);

	m_jumpClick = new cHudSprite(sprite_manager);
	m_jumpClick->Set_Image(pVideo->Get_Surface("extern/back.png"));
	m_jumpClick->Set_Pos(430, 250, 1);

	m_fire = new cHudSprite(sprite_manager);
	m_fire->Set_Image(pVideo->Get_Surface("extern/search.png"));
	m_fire->Set_Pos(400, 280, 1);

	m_fireClick = new cHudSprite(sprite_manager);
	m_fireClick->Set_Image(pVideo->Get_Surface("extern/search2.png"));
	m_fireClick->Set_Pos(400, 280, 1);
}

void Joystick::show()
{
	return;
//	m_dock->Set_Image( pFont->Render_Text( pFont->m_font_small, "Lines sdfsdfsfsdsf: ", blue ), 1, 1 );
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

	m_menu->Draw(NULL);
	m_jump->Draw(NULL);
	m_fire->Draw(NULL);
}

Joystick& Joystick::Instance()
{
	static Joystick s_joystick;
	return s_joystick;
}

}