#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include <SDL_mixer.h>

class Configuration;
struct Mix_Chunk;

class SoundEffects final {
public:
	SoundEffects(const Configuration& configuration);
	~SoundEffects();

	auto isEnabled() const noexcept { return m_enabled; }

	void enable() noexcept;
	void disable();

	void playUfo();
	void playShot();
	void playUfoDie();
	void playPlayerDie();
	void playInvaderDie();
	void playExtend();
	void playWalk1();
	void playWalk2();
	void playWalk3();
	void playWalk4();

private:
	const Configuration& m_configuration;

	bool m_enabled = false;

	std::shared_ptr<Mix_Chunk> m_ufoChunk;
	std::shared_ptr<Mix_Chunk> m_shotChunk;
	std::shared_ptr<Mix_Chunk> m_ufoDieChunk;
	std::shared_ptr<Mix_Chunk> m_playerDieChunk;
	std::shared_ptr<Mix_Chunk> m_invaderDieChunk;
	std::shared_ptr<Mix_Chunk> m_extendChunk;
	std::shared_ptr<Mix_Chunk> m_walk1Chunk;
	std::shared_ptr<Mix_Chunk> m_walk2Chunk;
	std::shared_ptr<Mix_Chunk> m_walk3Chunk;
	std::shared_ptr<Mix_Chunk> m_walk4Chunk;

	static void throwMixException(const std::string& failure) {
		auto reason = ::Mix_GetError();
		throw std::runtime_error(failure + reason);
	}

	static void verifyMixCall(int returned, const std::string& failure) {
		if (returned < 0) {
			throwMixException(failure);
		}
	}

	void open();
	void close() noexcept;

	void maybePlayEffect(int channel, Mix_Chunk* effect);
	static void playEffect(int channel, Mix_Chunk* effect);

	std::shared_ptr<Mix_Chunk> loadEffect(const std::string& name) const;
};
