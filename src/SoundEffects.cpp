#include "stdafx.h"
#include "SoundEffects.h"
#include "Configuration.h"

SoundEffects::SoundEffects(const Configuration& configuration)
:	m_configuration(configuration) {

	::Mix_Init(0);

	verifyMixCall(
		::Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 6, 4096),
		"Unable to open audio: ");

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

std::shared_ptr<Mix_Chunk> SoundEffects::loadEffect(const std::string& name) const {
	auto soundDirectory = m_configuration.getSoundDirectory();
	auto extension = ".wav";
	auto path = soundDirectory + "/" + name + extension;
	auto chunk = Mix_LoadWAV(path.c_str());
	if (chunk == nullptr) {
		auto message = "Unable to load mix chunk (" + name + "): ";
		throwMixException(message);
	}
	return std::shared_ptr<Mix_Chunk>(chunk, ::Mix_FreeChunk);
}

void SoundEffects::playEffect(int channel, Mix_Chunk* effect) {
	verifyMixCall(
		Mix_PlayChannel(channel, effect, 0),
		"Unable to play sound effect: ");
}

SoundEffects::~SoundEffects() {
	::Mix_CloseAudio();
	::Mix_Quit();
}

void SoundEffects::playUfo() {
	playEffect(1, m_ufoChunk.get());
}

void SoundEffects::playShot() {
	playEffect(2, m_shotChunk.get());
}

void SoundEffects::playUfoDie() {
	playEffect(3, m_ufoDieChunk.get());
}

void SoundEffects::playPlayerDie() {
	playEffect(4, m_playerDieChunk.get());
}

void SoundEffects::playInvaderDie() {
	playEffect(5, m_InvaderDieChunk.get());
}

void SoundEffects::playWalk1() {
	playEffect(0, m_walk1Chunk.get());
}

void SoundEffects::playWalk2() {
	playEffect(0, m_walk2Chunk.get());
}

void SoundEffects::playWalk3() {
	playEffect(0, m_walk3Chunk.get());
}

void SoundEffects::playWalk4() {
	playEffect(0, m_walk4Chunk.get());
}
