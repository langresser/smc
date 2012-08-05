#include "joystick.h"
#include "../core/sprite_manager.h"
#include "../objects/sprite.h"
#include "../video/font.h"
#include "../video/color.h"

namespace SMC {

void Joystick::createJoystick(cSprite_Manager *sprite_manager)
{
	m_dock = new cHudSprite(sprite_manager);
//	m_dock->Set_Image(pVideo->Get_Surface("extern/jsbg.png"));
	m_dock->Set_Image(pVideo->Get_Surface("extern/audio.png"));
	m_dock->Set_Pos( 150, 100 * 0.97f, 1 );
	m_spriteManager = sprite_manager;
}

void Joystick::show()
{
//	m_dock->Set_Image( pFont->Render_Text( pFont->m_font_small, "Lines sdfsdfsfsdsf: ", blue ), 1, 1 );
	m_dock->Draw( NULL );
}

Joystick& Joystick::Instance()
{
	static Joystick s_joystick;
	return s_joystick;
}

}