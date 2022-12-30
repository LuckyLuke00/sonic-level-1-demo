#pragma once
#include "Enemy.h"

class Motobug final :
	public Enemy
{
public:
	explicit Motobug(const std::string& motobugPath, const std::string& minMaxPosPath, const std::vector<std::vector<Point2f>>* levelVerticies, Player* player, const Camera* camera);
	~Motobug();

	void Update(float& elapsedSec, const float& fpsTarget);
	void Draw() const;
	void Reset();
private:
	const float m_ExplosionSpriteSize;
	const float m_Gravity;
	const std::string m_MinMaxPosPath;
	const std::vector<std::vector<Point2f>>* m_LevelVerticies;

	std::vector<bool> m_HasSpawned;
	std::vector<bool> m_IsOnGround;
	std::vector<float> m_MovingLeft; // Will be -1 when moving left and 1 when moving right
	std::vector<Linef> m_GroundSensors;
	std::vector<SpriteManager*> m_ExplosionSprite;
	std::vector<std::pair<float, float>> m_MinMaxPositions;

	Texture* m_pExplosionTexture;

	void DestroyMotobug(const size_t& idx);
	void HandleCollision();
	void HandlePlayerCollision();
	void Init();
	void SetMinMaxPositions(const std::string& minMaxPosPath);
};
