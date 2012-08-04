/***************************************************************************
 * audio.cpp  -  Audio class
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

#include "../audio/audio.h"
#include "../core/game_core.h"
#include "../level/level.h"
#include "../overworld/overworld.h"
#include "../user/preferences.h"
#include "../core/i18n.h"
#include "../core/filesystem/filesystem.h"

namespace SMC
{

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

void Finished_Sound( const int channel )
{
	// find the finished sound and free the data
	for( AudioSoundList::iterator itr = pAudio->m_active_sounds.begin(); itr != pAudio->m_active_sounds.end(); ++itr )
	{
		cAudio_Sound *obj = (*itr);

		if( obj->m_channel == channel )
		{
			obj->Finished();
		}
	}
}

/* *** *** *** *** *** *** *** *** Audio Sound *** *** *** *** *** *** *** *** *** */

cAudio_Sound :: cAudio_Sound( void )
{
	m_data = NULL;
	m_channel = -1;
	m_resource_id = -1;
}

cAudio_Sound :: ~cAudio_Sound( void )
{
	Free();
}

void cAudio_Sound :: Load( cSound *data )
{
	Free();
	
	m_data = data;
}

void cAudio_Sound :: Free( void )
{
	Stop();

	if( m_data )
	{
		m_data = NULL;
	}
	
	m_channel = -1;
	m_resource_id = -1;
}

void cAudio_Sound :: Finished( void )
{
	m_channel = -1;
}

int cAudio_Sound :: Play( int use_res_id /* = -1 */, int loops /* = 0 */ )
{
	if( !m_data || !m_data->m_chunk )
	{
		return 0;
	}

	if( use_res_id >= 0 )
	{
		for( AudioSoundList::iterator itr = pAudio->m_active_sounds.begin(); itr != pAudio->m_active_sounds.end(); ++itr )
		{
			// get object pointer
			cAudio_Sound *obj = (*itr);

			// skip self
			if( !obj || obj->m_channel == m_channel )
			{
				continue;
			}

			// stop Sounds using the given resource id
			if( obj->m_resource_id == use_res_id )
			{
				obj->Stop();
			}
		}
	}

	m_resource_id = use_res_id;
	// play sound
	m_channel = Mix_PlayChannel( -1, m_data->m_chunk, loops );
	// add callback if sound finished playing
	Mix_ChannelFinished( &Finished_Sound );

	return m_channel;
}

void cAudio_Sound :: Stop( void )
{
	// if not loaded or not playing
	if( !m_data || m_channel < 0 )
	{
		return;
	}
	
	Mix_HaltChannel( m_channel );
	m_channel = -1;
}

/* *** *** *** *** *** *** *** *** Audio *** *** *** *** *** *** *** *** *** */

cAudio :: cAudio( void )
{
	m_initialised = 0;
	m_sound_enabled = 0;
	m_music_enabled = 0;

	m_debug = 0;

	m_sound_volume = cPreferences::m_sound_volume_default;
	m_music_volume = cPreferences::m_music_volume_default;

	m_music = NULL;
	m_music_old = NULL;

	m_max_sounds = 0;

	m_audio_buffer = 4096; // below 2048 can be choppy
	m_audio_channels = MIX_DEFAULT_CHANNELS; // 1 = Mono, 2 = Stereo
}

cAudio :: ~cAudio( void )
{
	Close();
}

bool cAudio :: Init( void )
{
	// Get current device parameters
	int dev_frequency = 0;
	Uint16 dev_format = 0;
	int dev_channels = 0;
	int numtimesopened = Mix_QuerySpec( &dev_frequency, &dev_format, &dev_channels );

	bool sound = pPreferences->m_audio_sound;
	bool music = pPreferences->m_audio_music;

	// if no change
	if( numtimesopened && m_music_enabled == music && m_sound_enabled == sound)
	{
		return 1;
	}

	Close();

	// if no audio
	if( !music && !sound )
	{
		return 1;
	}

	// if audio system is not initialized
	if( !m_initialised )
	{
		if( m_debug )
		{
			printf( "Initializing Audio System - Buffer %i, Frequency %i, Speaker Channels %i\n", m_audio_buffer, AUDIO_HZ, m_audio_channels );
		}

		/*	Initializing preferred Audio System specs with Mixer Standard format (Stereo)
		*
		*	frequency	: Output sampling frequency in samples per second (Hz).
		*	format		: Output sample format.
		*	channels	: Number of sound channels in output. 2 for stereo and 1 for mono.
		*	chunk size	: Bytes used per output sample.
		*/

		if( Mix_OpenAudio( AUDIO_HZ, MIX_DEFAULT_FORMAT, m_audio_channels, m_audio_buffer ) < 0 ) 
		{
			printf( "Warning : Could not init 16-bit Audio\n- Reason : %s\n", SDL_GetError() );
			return 0;
		}

		numtimesopened = Mix_QuerySpec( &dev_frequency, &dev_format, &dev_channels );

		if( !numtimesopened )
		{
			printf( "Mix_QuerySpec failed: %s\n", Mix_GetError() );
		}
		else
		{
			// different format
			if( dev_format != MIX_DEFAULT_FORMAT )
			{
				const char *format_str;

				switch( dev_format )
				{
					case AUDIO_U8:
						format_str = "U8";
						break;
					case AUDIO_S8:
						format_str = "S8";
						break;
					case AUDIO_U16LSB:
						format_str = "U16LSB";
						break;
					case AUDIO_S16LSB:
						format_str = "S16LSB";
						break;
					case AUDIO_U16MSB:
						format_str = "U16MSB";
						break;
					case AUDIO_S16MSB:
						format_str = "S16MSB";
						break;
					default:
						format_str = "Unknown";
						break;
				}

				printf( "Warning : got different format %s\n", format_str );
			}

			// different amount of channels
			if( m_audio_channels != dev_channels )
			{
				printf( "Warning : different channels got %d but requested %d\n", dev_channels, m_audio_channels );
			}
		}

		m_initialised = 1;
	}


	if( m_debug )
	{
		printf( "Audio Sound Channels available : %d\n", Mix_AllocateChannels( -1 ) );
	}

	// music initialization
	if( music && !m_music_enabled )
	{
		m_music_enabled = 1;

		// set music volume
		Set_Music_Volume( m_music_volume );
	}
	// music de-initialization
	else if( !music && m_music_enabled )
	{
		Halt_Music();

		m_music_enabled = 0;
	}

	// sound initialization
	if( sound && !m_sound_enabled )
	{
		m_sound_enabled = 1;

		// create sound array
		Set_Max_Sounds();
		// set sound volume
		Set_Sound_Volume( m_sound_volume );
	}
	// sound de-initialization
	else if( !sound && m_sound_enabled )
	{
		Stop_Sounds();

		m_sound_enabled = 0;
	}

	return 1;
}

void cAudio :: Close( void )
{
	if( m_initialised )
	{
		if( m_debug )
		{
			printf( "Closing Audio System\n" );
		}

		if( m_sound_enabled )
		{
			Stop_Sounds();

			// clear sounds
			for( AudioSoundList::iterator itr = m_active_sounds.begin(); itr != m_active_sounds.end(); ++itr )
			{
				delete *itr;
			}

			m_active_sounds.clear();

			Mix_AllocateChannels( 0 );
			m_max_sounds = 0;
			m_sound_enabled = 0;
		}

		if( m_music_enabled )
		{
			Halt_Music();

			if( m_music )
			{
				Mix_FreeMusic( m_music );
				m_music = NULL;
			}

			if( m_music_old )
			{
				Mix_FreeMusic( m_music_old );
				m_music_old = NULL;
			}

			m_music_enabled = 0;
		}

		Mix_CloseAudio();

		m_initialised = 0;
	}
}

void cAudio :: Set_Max_Sounds( unsigned int limit /* = 10 */ )
{
	if( !m_initialised || !m_sound_enabled )
	{
		return;
	}

	// if limit is too small set it to the minimum
	if( limit < 5 )
	{
		limit = 5;
	}

	m_max_sounds = limit;

	// remove exceeding sounds
	AudioSoundList::iterator last_itr;

	while( m_active_sounds.size() > m_max_sounds )
	{
		last_itr = (m_active_sounds.end() - 1);

		// delete data
		delete *(last_itr);
		// erase from list
		m_active_sounds.erase( last_itr );
	}

	// change channels managed by the mixer
	Mix_AllocateChannels( m_max_sounds );

	if( m_debug )
	{
		printf( "Audio Sound Channels changed : %d\n", Mix_AllocateChannels( -1 ) );
	}
}

cSound *cAudio :: Get_Sound_File( std::string filename ) const
{
	if( !m_initialised || !m_sound_enabled )
	{
		return NULL;
	}

	// not available
	if( !File_Exists( filename ) )
	{
		// add sound directory
		if( filename.find( DATA_DIR "/" GAME_SOUNDS_DIR "/" ) == std::string::npos )
		{
			filename.insert( 0, DATA_DIR "/" GAME_SOUNDS_DIR "/" );
		}
	}

	cSound *sound = pSound_Manager->Get_Pointer( filename );

	// if not already cached
	if( !sound )
	{
		sound = new cSound();

		// loaded sound
		if( sound->Load( filename ) )
		{
			pSound_Manager->Add( sound );

			if( m_debug )
			{
				printf( "Loaded sound file : %s\n", filename.c_str() );
			}
		}
		// failed loading
		else
		{
			printf( "Could not load sound file : %s \nReason : %s\n", filename.c_str(), SDL_GetError() );
			
			delete sound;
			return NULL;
		}
	}

	return sound;
}

bool cAudio :: Play_Sound( std::string filename, int res_id /* = -1 */, int volume /* = -1 */, int loops /* = 0 */ )
{
	if( !m_initialised || !m_sound_enabled )
	{
		return 0;
	}

	// not available
	if( !File_Exists( filename ) )
	{
		// add sound directory
		if( filename.find( DATA_DIR "/" GAME_SOUNDS_DIR "/" ) == std::string::npos )
		{
			filename.insert( 0, DATA_DIR "/" GAME_SOUNDS_DIR "/" );
		}

		// not found
		if( !File_Exists( filename ) )
		{
			printf( "Could not find sound file : %s\n", filename.c_str() );
			return 0;
		}
	}

	cSound *sound_data = Get_Sound_File( filename );

	// failed loading
	if( !sound_data )
	{
		printf( "Warning : Could not load sound file : %s\n", filename.c_str() );
		return 0;
	}

	// create channel
	cAudio_Sound *sound = Create_Sound_Channel();

	if( !sound )
	{
		// no free channel available
		return 0;
	}

	// load data
	sound->Load( sound_data );
	// play
	sound->Play( res_id, loops );

	// failed to play
	if( sound->m_channel < 0 )
	{
		if( m_debug )
		{
			printf( "Could not play sound file : %s\n", filename.c_str() );
		}

		return 0;
	}
	// playing successfully
	else
	{
		// volume is out of range
		if( volume > MIX_MAX_VOLUME )
		{
			printf( "PlaySound Volume is out of range : %d\n", volume );
			volume = m_sound_volume;
		}
		// no volume is given
		else if( volume < 0 )
		{
			volume = m_sound_volume;
		}

		// set volume
		Mix_Volume( sound->m_channel, volume );
	}

	return 1;
}

bool cAudio :: Play_Music( std::string filename, int loops /* = 0 */, bool force /* = 1 */, unsigned int fadein_ms /* = 0 */ )
{
	if( filename.find( DATA_DIR "/" GAME_MUSIC_DIR "/" ) == std::string::npos )
	{
		filename.insert( 0, DATA_DIR "/" GAME_MUSIC_DIR "/" );
	}

	// no valid file
	if( !File_Exists( filename ) )
	{
		printf( "Couldn't find music file : %s\n", filename.c_str() );
		return 0;
	}

	// save music filename
	m_music_filename = filename;

	if( !m_music_enabled || !m_initialised )
	{
		return 0;
	}

	// if music is stopped resume it
	Resume_Music();

	// if no music is playing or force to play the given music
	if( !Is_Music_Playing() || force ) 
	{
		// stop and free current music
		if( m_music )
		{
			Halt_Music();
			Mix_FreeMusic( m_music );
		}
		// free old music
		if( m_music_old )
		{
			Mix_FreeMusic( m_music_old );
			m_music_old = NULL;
		}

		// load the given music
		m_music = Mix_LoadMUS( filename.c_str() );

		// loaded
		if( m_music )
		{
			// no fade in
			if( !fadein_ms )
			{
				Mix_PlayMusic( m_music, loops );
			}
			// fade in
			else
			{
				Mix_FadeInMusic( m_music, loops, fadein_ms );
			}
		}
		// not loaded
		else 
		{
			if( m_debug )
			{
				printf( "Couldn't load music file : %s\n", filename.c_str() );
			}

			// failed to play
			return 0;
		}
	}
	// music is playing and is not forced
	else
	{
		// if music is loaded
		if( m_music )
		{
			// if old music is loaded free the wanted next playing music data
			if( m_music_old )
			{
				Mix_FreeMusic( m_music );
				m_music = NULL;
			}
			// if no old music move current to old music
			else
			{
				m_music_old = m_music;
				m_music = NULL;
			}
		}

		// load the wanted next playing music
		m_music = Mix_LoadMUS( filename.c_str() );
	}
	
	return 1;
}

cAudio_Sound *cAudio :: Get_Playing_Sound( std::string filename )
{
	if( !m_sound_enabled || !m_initialised )
	{
		return NULL;
	}

	// add sound directory
	if( filename.find( DATA_DIR "/" GAME_SOUNDS_DIR "/" ) == std::string::npos )
	{
		filename.insert( 0, DATA_DIR "/" GAME_SOUNDS_DIR "/" );
	}

	// get all sounds
	for( AudioSoundList::const_iterator itr = m_active_sounds.begin(); itr != m_active_sounds.end(); ++itr )
	{
		// get object pointer
		cAudio_Sound *obj = (*itr);

		// if not playing
		if( obj->m_channel < 0 )
		{
			continue;
		}

		// found it
		if( obj->m_data->m_filename.compare( filename ) == 0 )
		{
			// return first found
			return obj;
		}
	}

	// not found
	return NULL;
}

cAudio_Sound *cAudio :: Create_Sound_Channel( void )
{
	// get all sounds
	for( AudioSoundList::iterator itr = m_active_sounds.begin(); itr != m_active_sounds.end(); ++itr )
	{
		// get object pointer
		cAudio_Sound *obj = (*itr);

		// if not playing
		if( obj->m_channel < 0 )
		{
			// found a free channel
			obj->Free();
			return obj;
		}
	}

	// if not maximum sounds
	if( m_active_sounds.size() < m_max_sounds )
	{
		cAudio_Sound *sound = new cAudio_Sound();
		m_active_sounds.push_back( sound );
		return sound;
	}

	// none found
	return NULL;
}

void cAudio :: Toggle_Music( void )
{
	pPreferences->m_audio_music = !pPreferences->m_audio_music;
	Init();

	// play music
	if( m_music_enabled && !m_music_filename.empty() )
	{
		Play_Music( m_music_filename, -1, 1, 2000 );
	}
}

void cAudio :: Toggle_Sounds( void )
{
	pPreferences->m_audio_sound = !pPreferences->m_audio_sound;
	Init();

	// play test sound
	if( m_sound_enabled )
	{
		Play_Sound( "audio_on.ogg" );
	}
}

void cAudio :: Pause_Music( void ) const
{
	if( !m_music_enabled || !m_initialised )
	{
		return;
	}

	// if music is playing
	if( Mix_PlayingMusic() )
	{
		Mix_PauseMusic();
	}
}

void cAudio :: Resume_Sound( int channel /* = -1 */ ) const
{
	if( !m_sound_enabled || !m_initialised )
	{
		return;
	}

	// resume playback on all previously active channels
	Mix_Resume( channel );
}

void cAudio :: Resume_Music( void ) const
{
	if( !m_music_enabled || !m_initialised )
	{
		return;
	}

	// if music is paused
	if( Mix_PausedMusic() )
	{
		Mix_ResumeMusic();
	}
}

void cAudio :: Fadeout_Sounds( unsigned int ms /* = 200 */, int channel /* = -1 */, bool overwrite_fading /* = 0 */ ) const
{
	if( !m_sound_enabled || !m_initialised )
	{
		return;
	}

	// if not playing
	if( Mix_Playing( channel ) <= 0 )
	{
		return;
	}

	// Do not fade-out the sound again
	if( channel >= 0 && !overwrite_fading && Is_Sound_Fading( channel ) == MIX_FADING_OUT )
	{
		return;
	}

	Mix_FadeOutChannel( channel, ms );
}

void cAudio :: Fadeout_Sounds( unsigned int ms, std::string filename, bool overwrite_fading /* = 0 */ )
{
	if( !m_sound_enabled || !m_initialised )
	{
		return;
	}

	// add sound directory
	if( filename.find( DATA_DIR "/" GAME_SOUNDS_DIR "/" ) == std::string::npos )
	{
		filename.insert( 0, DATA_DIR "/" GAME_SOUNDS_DIR "/" );
	}

	// get all sounds
	for( AudioSoundList::const_iterator itr = m_active_sounds.begin(); itr != m_active_sounds.end(); ++itr )
	{
		// get object pointer
		const cAudio_Sound *obj = (*itr);

		// filename does not match
		if( obj->m_data->m_filename.compare( filename ) != 0 )
		{
			continue;
		}

		// Do not fade out the sound again
		if( !overwrite_fading && Is_Sound_Fading( obj->m_channel ) == MIX_FADING_OUT )
		{
			continue;
		}

		Mix_FadeOutChannel( obj->m_channel, ms );
	}
}

void cAudio :: Fadeout_Music( unsigned int ms /* = 500 */, bool overwrite_fading /* = 0 */ ) const
{
	if( !m_music_enabled || !m_initialised )
	{
		return;
	}

	// if music is currently not playing
	if( !Mix_PlayingMusic() )
	{
		return;
	}

	Mix_Fading status = Is_Music_Fading();

	// if already fading out
	if( status == MIX_FADING_OUT )
	{
		// don't fade the music out again
		if( !overwrite_fading )
		{
			return;
		}
	} 
	// if fading in
	else if( status == MIX_FADING_IN )
	{
		// Can't stop fade-in with SDL_Mixer and fade-out is ignored when fading in
		Halt_Music();
		return;
	}

	if( !Mix_FadeOutMusic( ms ) )
	{
		// if it failed stop the music
		Halt_Music();
	}
}

void cAudio :: Set_Music_Position( float position ) const
{
	if( !m_music_enabled || !m_initialised || Is_Music_Fading() == MIX_FADING_OUT )
	{
		return;
	}

	Mix_SetMusicPosition( position );
}

Mix_Fading cAudio :: Is_Music_Fading( void ) const
{
	if( !m_music_enabled || !m_initialised )
	{
		return MIX_NO_FADING;
	}

	return Mix_FadingMusic();
}

Mix_Fading cAudio :: Is_Sound_Fading( int sound_channel ) const
{
	if( !m_sound_enabled || !m_initialised || sound_channel < 0 )
	{
		return MIX_NO_FADING;
	}

	return Mix_FadingChannel( sound_channel );
}

bool cAudio :: Is_Music_Paused( void ) const
{
	if( !m_music_enabled || !m_initialised )
	{
		return 0;
	}
	
	if( Mix_PausedMusic() )
	{
		return 1;
	}
	
	return 0;
}

bool cAudio :: Is_Music_Playing( void ) const
{
	if( !m_music_enabled || !m_initialised )
	{
		return 0;
	}
	
	if( Mix_PlayingMusic() )
	{
		return 1;
	}
	
	return 0;
}

void cAudio :: Halt_Sounds( int channel /* = -1 */ ) const
{
	if( !m_sound_enabled || !m_initialised )
	{
		return;
	}

	// Check all Channels
	if( Mix_Playing( channel ) )
	{
		Mix_HaltChannel( channel );
	}
}

void cAudio :: Halt_Music( void ) const
{
	if( !m_initialised )
	{
		return;
	}

	// Checks if music is playing
	if( Mix_PlayingMusic() )
	{
		Mix_HaltMusic();
	}
}

void cAudio :: Stop_Sounds( void ) const
{
	if( !m_initialised )
	{
		return;
	}

	// Stop all channels
	if( Mix_Playing( -1 ) )
	{
		Mix_HaltChannel( -1 );
	}
}

void cAudio :: Set_Sound_Volume( Uint8 volume, int channel /* = -1 */ ) const
{
	// not active
	if( !m_initialised )
	{
		return;
	}

	// out of range
	if( volume > MIX_MAX_VOLUME )
	{
		volume = MIX_MAX_VOLUME;
	}

	Mix_Volume( channel, volume );
}

void cAudio :: Set_Music_Volume( Uint8 volume ) const
{
	// not active
	if( !m_initialised )
	{
		return;
	}

	// out of range
	if( volume > MIX_MAX_VOLUME )
	{
		volume = MIX_MAX_VOLUME;
	}

	Mix_VolumeMusic( volume );
}

void cAudio :: Update( void )
{
	if( !m_initialised )
	{
		return;
	}

	// if music is enabled
	if( m_music_enabled )
	{
		// if no music is playing
		if( !Mix_PlayingMusic() && m_music ) 
		{
			Mix_PlayMusic( m_music, 0 );

			// delete old music if available
			if( m_music_old )
			{
				Mix_FreeMusic( m_music_old );
				m_music_old = NULL;
			}
		}
	}
}

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

cAudio *pAudio = NULL;

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */

} // namespace SMC
