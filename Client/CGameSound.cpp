#include "stdafx.h"
#include "CGameSound.h"
CGameSound::CGameSound()
{
	result = FMOD::System_Create(&soundSystem);
	result = soundSystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
	result = soundSystem->createSound("Sound/Explosion.wav", FMOD_DEFAULT, 0, &BackGroundSound);
	result = BackGroundSound->setMode(FMOD_LOOP_NORMAL);
	//result = soundSystem->playSound(BackGroundSound, 0, true, &BackGroundChannel);
	BackGroundChannel->setVolume(0.5f);
}