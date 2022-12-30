#include "pch.h"
#include "SoundStream.h"
#include <iostream>

SoundStream::SoundStream(const std::string& path)
	:m_pMixMusic{ Mix_LoadMUS(path.c_str()) }
{
	if (m_pMixMusic == nullptr)
	{
		const std::string errorMsg = "SoundStream: Failed to load " + path + ",\nSDL_mixer Error: " + Mix_GetError();
		std::cerr << errorMsg;
	}
}

SoundStream::~SoundStream()
{
	Mix_FreeMusic(m_pMixMusic);
	m_pMixMusic = nullptr;
}

bool SoundStream::IsLoaded() const
{
	return m_pMixMusic != nullptr;
}

bool SoundStream::Play(bool repeat) const
{
	if (m_pMixMusic != nullptr)
	{
		const int result{ Mix_PlayMusic(m_pMixMusic, repeat ? -1 : 1) };
		return result == 0 ? true : false;
	}
	else
	{
		return false;
	}
}

void SoundStream::Stop()
{
	Mix_HaltMusic();
}
void SoundStream::Pause()
{
	Mix_PauseMusic();
}

void SoundStream::Resume()
{
	Mix_ResumeMusic();
}

int SoundStream::GetVolume()
{
	return Mix_VolumeMusic(-1);
}

bool SoundStream::IsPlaying()
{
	return Mix_PlayingMusic() == 0 ? false : true;
}

void SoundStream::SetVolume(int value)
{
	Mix_VolumeMusic(value);
}
