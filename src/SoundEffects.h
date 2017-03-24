#pragma once

#include <memory>
#include <stdexcept>

class Configuration;
struct Mix_Chunk;

class SoundEffects {
public:
	SoundEffects(const Configuration& configuration);
	~SoundEffects();

	void playUfo();
	void playShot();
	void playUfoDie();
	void playPlayerDie();
	void playInvaderDie();
	void playWalk1();
	void playWalk2();
	void playWalk3();
	void playWalk4();

private:
	const Configuration& m_configuration;

	std::shared_ptr<Mix_Chunk> m_ufoChunk;
	std::shared_ptr<Mix_Chunk> m_shotChunk;
	std::shared_ptr<Mix_Chunk> m_ufoDieChunk;
	std::shared_ptr<Mix_Chunk> m_playerDieChunk;
	std::shared_ptr<Mix_Chunk> m_InvaderDieChunk;
	std::shared_ptr<Mix_Chunk> m_walk1Chunk;
	std::shared_ptr<Mix_Chunk> m_walk2Chunk;
	std::shared_ptr<Mix_Chunk> m_walk3Chunk;
	std::shared_ptr<Mix_Chunk> m_walk4Chunk;

	static void throwMixException(const std::string& failure) {
		throw std::runtime_error(failure + ::Mix_GetError());
	}

	static void verifyMixCall(int returned, const std::string& failure) {
		if (returned < 0) {
			throwMixException(failure);
		}
	}

	static void playEffect(int channel, Mix_Chunk* effect);

	std::shared_ptr<Mix_Chunk> loadEffect(const std::string& name) const;
};
