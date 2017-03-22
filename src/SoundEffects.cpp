#include "stdafx.h"
#include "SoundEffects.h"
#include "Configuration.h"

/* set this to any of 512,1024,2048,4096              */
/* the lower it is, the more FPS shown and CPU needed */
//#define BUFFER 1024
//#define W 640 /* NEVER make this be less than BUFFER! */
//#define H 480
//#define H2 (H/2)
//#define H4 (H/4)
//#define Y(sample) (((sample)*H)/4/0x7fff)
//
//Sint16 stream[2][BUFFER * 2 * 2];
//int len = BUFFER * 2 * 2, done = 0, need_refresh = 0, bits = 0, which = 0,
//sample_size = 0, position = 0, rate = 0;
//SDL_Surface *s = NULL;
//Uint32 flips = 0;
//Uint32 black, white;
//float dy;


SoundEffects::SoundEffects(const Configuration& configuration)
:	m_configuration(configuration) {

	auto available = ::Mix_Init(~0);


	auto mix_open = ::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);

	m_ufoChunk = loadEffect("Ufo");
	m_shotChunk = loadEffect("Shot");
	m_playerDieChunk = loadEffect("BaseHit");
	m_ufoDieChunk = loadEffect("UfoHit");
	m_InvaderDieChunk = loadEffect("InvHit");
	m_walk1Chunk = loadEffect("Walk1");
	m_walk2Chunk = loadEffect("Walk2");
	m_walk3Chunk = loadEffect("Walk3");
	m_walk4Chunk = loadEffect("Walk4");
}

Mix_Chunk* SoundEffects::loadEffect(std::string name) const {
	auto soundDirectory = m_configuration.getSoundDirectory();
	auto extension = ".wav";
	auto path = soundDirectory + "/" + name + extension;
	return Mix_LoadWAV(path.c_str());
}

SoundEffects::~SoundEffects() {

	::Mix_CloseAudio();

	::Mix_FreeChunk(m_ufoChunk);
	::Mix_FreeChunk(m_shotChunk);
	::Mix_FreeChunk(m_ufoDieChunk);
	::Mix_FreeChunk(m_playerDieChunk);
	::Mix_FreeChunk(m_InvaderDieChunk);
	::Mix_FreeChunk(m_walk1Chunk);
	::Mix_FreeChunk(m_walk2Chunk);
	::Mix_FreeChunk(m_walk3Chunk);
	::Mix_FreeChunk(m_walk4Chunk);

	::Mix_Quit();
}

void SoundEffects::playUfo() {
	Mix_PlayChannel(-1, m_ufoChunk, 0);
}

void SoundEffects::playShot() {
	Mix_PlayChannel(-1, m_shotChunk, 0);
}

void SoundEffects::playUfoDie() {
	Mix_PlayChannel(-1, m_ufoDieChunk, 0);
}

void SoundEffects::playPlayerDie() {
	Mix_PlayChannel(-1, m_playerDieChunk, 0);
}

void SoundEffects::playInvaderDie() {
	Mix_PlayChannel(-1, m_InvaderDieChunk, 0);
}

void SoundEffects::playWalk1() {
	Mix_PlayChannel(-1, m_walk1Chunk, 0);
}

void SoundEffects::playWalk2() {
	Mix_PlayChannel(-1, m_walk2Chunk, 0);
}

void SoundEffects::playWalk3() {
	Mix_PlayChannel(-1, m_walk3Chunk, 0);
}

void SoundEffects::playWalk4() {
	Mix_PlayChannel(-1, m_walk4Chunk, 0);
}
