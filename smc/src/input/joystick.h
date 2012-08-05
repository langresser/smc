#pragma once
#include "SDL.h"
#include "../video/video.h"
#include "../gui/hud.h"

#define DIR_NONE 0
#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_RIGHT 4

namespace SMC
{

	class Joystick
	{
	public:
		static Joystick& Instance();
		Joystick() {};
		~Joystick() {};

	public:
		void createJoystick(cSprite_Manager *sprite_manager);
		void show();
	private:
		int m_dir;
		cHudSprite* m_dock;
		cHudSprite* m_up;
		cHudSprite* m_down;
		cHudSprite* m_left;
		cHudSprite* m_right;
		cSprite_Manager* m_spriteManager;
	};
}