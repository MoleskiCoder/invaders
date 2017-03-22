#pragma once

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

	Mix_Chunk* m_ufoChunk;
	Mix_Chunk* m_shotChunk;
	Mix_Chunk* m_ufoDieChunk;
	Mix_Chunk* m_playerDieChunk;
	Mix_Chunk* m_InvaderDieChunk;
	Mix_Chunk* m_walk1Chunk;
	Mix_Chunk* m_walk2Chunk;
	Mix_Chunk* m_walk3Chunk;
	Mix_Chunk* m_walk4Chunk;

	Mix_Chunk* loadEffect(std::string name) const;
};
