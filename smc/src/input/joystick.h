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
        void touchEvent(int type, int x, int y, int figureId);
	private:
        bool isTouchIn(int touchx, int touchy, int x, int y, int w, int h);
		int m_dir;
		cHudSprite* m_dock;
		cHudSprite* m_up;
		cHudSprite* m_down;
		cHudSprite* m_left;
		cHudSprite* m_right;

        bool m_showMenuBack;
        bool m_menuDown;
        bool m_jumpDown;
        bool m_fireDown;
		cHudSprite* m_menuBack;
		cHudSprite* m_menuBackClick;
		cHudSprite* m_menu;
		cHudSprite* m_menuClick;
		cHudSprite* m_jump;
		cHudSprite* m_jumpClick;
		cHudSprite* m_fire;
		cHudSprite* m_fireClick;
		cSprite_Manager* m_spriteManager;
	};
}