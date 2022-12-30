#pragma once
#include "GameObject.h"

class Player;
class SoundEffect;

class Rings final :
	public GameObject
{
public:
	explicit Rings(const std::string& locationFilePath, Player* player);

	Rings(const Rings& other) = delete;
	Rings& operator=(const Rings& other) = delete;
	Rings(Rings&& other) = delete;
	Rings& operator=(Rings&& other) = delete;

	~Rings() override;

	void HandleCollision();
	virtual void Update(float& elapsedSec, const float& fpsTarget) override;

	void Reset();
	void AddRings(const int& ringsToAdd);
private:
	bool m_HasBeenAwarded;
	const int m_RingBonusAmount;

	Player* m_pPlayer;
	SoundEffect* m_pRingLossSound;
	SoundEffect* m_pRingSound;

	bool AwardBonus();
};
