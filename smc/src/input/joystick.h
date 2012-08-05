#pragma once
#include "SDL.h"
#include "../video/video.h"
#include "../gui/hud.h"

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
		cHudSprite* m_dock;
		cSprite_Manager* m_spriteManager;
	};
}