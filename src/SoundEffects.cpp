#include "stdafx.h"
#include "SoundEffects.h"
#include "Configuration.h"

SoundEffects::SoundEffects(const Configuration& configuration)
:	m_configuration(configuration) {

	::Mix_Init(0);

	open();

	enable();

	verifyMixCall(
		::Mix_GroupChannels(0, 5, 0),
		"Unable to group sound channels: ");

	m_ufoChunk = loadEffect("Ufo");
	m_shotChunk = loadEffect("Shot");
	m_playerDieChunk = loadEffect("BaseHit");
	m_ufoDieChunk = loadEffect("UfoHit");
	m_invaderDieChunk = loadEffect("InvHit");
	m_extendChunk = loadEffect("Extend");
	m_walk1Chunk = loadEffect("Walk1");
	m_walk2Chunk = loadEffect("Walk2");
	m_walk3Chunk = loadEffect("Walk3");
	m_walk4Chunk = loadEffect("Walk4");
}

void SoundEffects::enable() {
	m_enabled = true;
}

void SoundEffects::disable() {
	verifyMixCall(
		::Mix_HaltGroup(0),
		"Unable to halt sound group: ");

	m_enabled = false;
}

void SoundEffects::open() {
	verifyMixCall(
		::Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 6, 4096),
		"Unable to open audio: ");
}

void SoundEffects::close() {
	::Mix_CloseAudio();
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
	return { chunk, ::Mix_FreeChunk };
}

void SoundEffects::playEffect(int channel, Mix_Chunk* effect) {
	verifyMixCall(
		::Mix_HaltChannel(channel),
		"Unable to halt channel: ");
	verifyMixCall(
		Mix_PlayChannel(channel, effect, 0),
		"Unable to play sound effect: ");
}

SoundEffects::~SoundEffects() {
	close();
	::Mix_Quit();
}

void SoundEffects::playUfo() {
	if (m_enabled)
		playEffect(1, m_ufoChunk.get());
}

void SoundEffects::playShot() {
	if (m_enabled)
		playEffect(2, m_shotChunk.get());
}

void SoundEffects::playUfoDie() {
	if (m_enabled)
		playEffect(3, m_ufoDieChunk.get());
}

void SoundEffects::playPlayerDie() {
	if (m_enabled)
		playEffect(4, m_playerDieChunk.get());
}

void SoundEffects::playInvaderDie() {
	if (m_enabled)
		playEffect(3, m_invaderDieChunk.get());
}

void SoundEffects::playExtend() {
	if (m_enabled)
		playEffect(5, m_extendChunk.get());
}

void SoundEffects::playWalk1() {
	if (m_enabled)
		playEffect(0, m_walk1Chunk.get());
}

void SoundEffects::playWalk2() {
	if (m_enabled)
		playEffect(0, m_walk2Chunk.get());
}

void SoundEffects::playWalk3() {
	if (m_enabled)
		playEffect(0, m_walk3Chunk.get());
}

void SoundEffects::playWalk4() {
	if (m_enabled)
		playEffect(0, m_walk4Chunk.get());
}
