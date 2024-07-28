#pragma once
#include <fmod.hpp>
#include <fmod_errors.h>

class CGameSound
{
public:
	FMOD::System* soundSystem;
	FMOD::Sound* BackGroundSound;
	FMOD::Channel* BackGroundChannel;
	FMOD_RESULT  result;
	void* extradriverdata = 0;
public:
	CGameSound();
	~CGameSound();
public:
	bool m_bStopSound = false;
	void PauseOpeningSound();
	void PlayOpeningSound();
};

