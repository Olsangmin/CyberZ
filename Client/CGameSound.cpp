#include "CGameSound.h"


//CGameSound::CGameSound(const char* path, bool loop) {
//	result = FMOD::System_Create(&soundSystem);
//
//
//	result = soundSystem->init(32, FMOD_INIT_NORMAL, extradriverdata);
//	result = soundSystem->createSound("Sound/Explosion.wav", FMOD_DEFAULT, 0, &BackGroundSound);
//	result = BackGroundSound->setMode(FMOD_LOOP_NORMAL);
//
//	result = soundSystem->playSound(FMOD_CHANNEL_FREE,BackGroundSound, true, &BackGroundChannel);
//	BackGroundChannel->setVolume(0.5f);
//}
//
//CGameSound::~CGameSound() {
//	result = BackGroundSound->release();
//}

//
//int CGameSound::Init() {
//    FMOD_System_Create(&g_sound_system);
//    FMOD_System_Init(g_sound_system, 32, FMOD_INIT_NORMAL, NULL);
//
//    return 0;
//}
//
//int CGameSound::Release() {
//    FMOD_System_Close(g_sound_system);
//    FMOD_System_Release(g_sound_system);
//
//    return 0;
//}
//
//
//int CGameSound::play() {
//    FMOD_System_PlaySound(g_sound_system,FMOD_CHANNEL_FREE ,m_sound, FALSE, &m_channel);
//    return 0;
//}
//
//int CGameSound::pause() {
//    FMOD_Channel_SetPaused(m_channel, true);
//
//    return 0;
//}
//
//int CGameSound::resume() {
//    FMOD_Channel_SetPaused(m_channel, false);
//
//    return 0;
//}
//
//int CGameSound::stop() {
//    FMOD_Channel_Stop(m_channel);
//
//    return 0;
//}
//
//
//int CGameSound::Update() {
//    FMOD_Channel_IsPlaying(m_channel, &m_bool);
//
//    if (m_bool) {
//        FMOD_System_Update(g_sound_system);
//    }
//
//    return 0;
//}
