#pragma once
#include "GameObject.h"
class Player;

class FallingPlatform final :
	public GameObject
{
public:
	explicit FallingPlatform(const std::string& startPosPath, Player* player);

	FallingPlatform(const FallingPlatform& other) = delete;
	FallingPlatform& operator=(const FallingPlatform& other) = delete;
	FallingPlatform(FallingPlatform&& other) = delete;
	FallingPlatform& operator=(FallingPlatform&& other) = delete;

	bool HandleCollision();
	virtual void Update(float& elapsedSec, const float& fpsTarget) override;
	virtual void Reset() override;
private:
	const float m_FallingPlatformBottomPosY;
	const float m_FallingSpeed;
	const float m_FallTime;
	const float m_ResetTime;

	std::vector<bool> m_StartTimer;
	std::vector<float> m_FallTimer;
	std::vector<float> m_ResetTimer;

	Player* m_pPlayer;
};
