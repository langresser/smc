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
}

void Joystick::show()
{
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
}

Joystick& Joystick::Instance()
{
	static Joystick s_joystick;
	return s_joystick;
}

}