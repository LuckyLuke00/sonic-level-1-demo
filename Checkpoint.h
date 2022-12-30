#pragma once
#include "GameObject.h"

class Player;
class SoundEffect;

class Checkpoint final :
	public GameObject
{
public:
	explicit Checkpoint(const std::vector<Point2f>& locations, Player* player);

	Checkpoint(const Checkpoint& other) = delete;
	Checkpoint& operator=(const Checkpoint& other) = delete;
	Checkpoint(Checkpoint&& other) = delete;
	Checkpoint& operator=(Checkpoint&& other) = delete;

	~Checkpoint() override;

	void HandleCollision();
	virtual void Reset() override;
private:
	std::vector<bool> m_CheckpointHit;

	Player* m_Player;
	SoundEffect* m_pCheckpointSound;
};
