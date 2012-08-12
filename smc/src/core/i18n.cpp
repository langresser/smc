/***************************************************************************
 * i18n.cpp  -  internationalization with gettext
 *
 * Copyright (C) 2008 - 2011 Florian Richter
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
#include "../core/i18n.h"
#include "SDL.h"

#ifdef WIN32
#include <windows.h>
#endif

namespace SMC
{

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

void I18N_Init( void )
{
// 	const char *sys_locale = setlocale( LC_ALL, "" );
// 	const char* language = SDL_getenv("LANGUAGE");
// 
// 	if( sys_locale == NULL )
// 	{
// 		debug_print( "Failed to set translation locale\n" );
// 	}
// 	else
// 	{
// 		debug_print( "Translation locale is %s\n", sys_locale );
// 	}
// 
// 	const char *textdomain_directory = bindtextdomain( CAPTION, DATA_DIR "/" GAME_TRANSLATION_DIR );
// 
// 	if( !textdomain_directory )
// 	{
// 		printf( "Warning: bindtextdomain failed for %s\n", DATA_DIR "/" GAME_TRANSLATION_DIR );
// 	}
// 
// 	const char *textdomain_codeset = bind_textdomain_codeset( CAPTION, language );
// 	const char *textdomain_default = textdomain( CAPTION );
// 
// 	debug_print( "Translation support with gettext set to:\n\tDirectory %s\n\tCodeset: %s\n\tText domain: %s\n",
// 		textdomain_directory, textdomain_codeset, textdomain_default );
}

#ifdef _WIN32
int Get_Windows_Primary_Language( const std::string &language )
{
	if( language.compare( "en" ) == 0 )
	{
		return LANG_ENGLISH;
	}
	else if( language.compare( "de" ) == 0 )
	{
		return LANG_GERMAN;
	}
	else if( language.compare( "el" ) == 0 )
	{
		return LANG_GREEK;
	}
	else if( language.compare( "es" ) == 0 )
	{
		return LANG_SPANISH;
	}
	else if( language.compare( "fi" ) == 0 )
	{
		return LANG_FINNISH;
	}
	else if( language.compare( "fr" ) == 0 )
	{
		return LANG_FRENCH;
	}
	else if( language.compare( "it" ) == 0 )
	{
		return LANG_ITALIAN;
	}
	else if( language.compare( "nb" ) == 0 )
	{
		return LANG_NORWEGIAN;
	}
	else if( language.compare( "nl" ) == 0 )
	{
		return LANG_DUTCH;
	}
	else if( language.compare( "nn" ) == 0 )
	{
		return LANG_NORWEGIAN;
	}
	else if( language.compare( "pl" ) == 0 )
	{
		return LANG_POLISH;
	}
	else if( language.compare( "ru" ) == 0 )
	{
		return LANG_RUSSIAN;
	}
	else if( language.compare( "sv" ) == 0 )
	{
		return LANG_SWEDISH;
	}
	else if( language.compare( "zh_TW" ) == 0 )
	{
		return LANG_CHINESE;
	}

	return LANG_NEUTRAL;
}

int Get_Windows_Sub_Language( const std::string &language )
{
	if( language.compare( "nn" ) == 0 )
	{
		return SUBLANG_NORWEGIAN_NYNORSK;
	}
	else if( language.compare( "zh_TW" ) == 0 )
	{
		return SUBLANG_CHINESE_TRADITIONAL;
	}

	return SUBLANG_NEUTRAL;
}
#endif

void I18N_Set_Language( const std::string &language )
{
//	SDL_setenv("LANGUAGE", language.c_str(), 1);
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC