#pragma once
#include "GameObject.h"
class Player;

class MovingPlatform final :
	public GameObject
{
public:
	explicit MovingPlatform(const std::string& startPosPath, const std::string& minMaxPosPath, Player* player);

	bool HandleCollision();
	virtual void Update(float& elapsedSec, const float& fpsTarget) override;
private:
	const float m_Speed;

	std::vector<float> m_Speeds;
	std::vector<std::pair<Point2f, Point2f>> m_MinMaxPositions;

	Player* m_pPlayer;

	void Initialize(const std::string& minMaxPosPath);
};
