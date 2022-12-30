#pragma once
#include "GameObject.h"

class SoundEffect;
class Player;

class Spikes final :
	public GameObject
{
public:
	explicit Spikes(const std::string& BigSpikesLocationFilePath, const std::string& SmallSpikesLocationFilePath, Player* player);

	Spikes(const Spikes& other) = delete;
	Spikes& operator=(const Spikes& other) = delete;
	Spikes(Spikes&& other) = delete;
	Spikes& operator=(Spikes&& other) = delete;

	~Spikes() override;

	virtual void Draw() const override;
	virtual void Update(float& elapsedSec, const float& fpsTarget) override;

	bool HandleCollision();

	Rectf m_HitSpike;
private:
	GameObject m_SmallSpikes;
	Player* m_pPlayer;
	SoundEffect* m_SpikeDeathSound;

	void CheckDeath() const;
};
