#pragma once
#include "Enemy.h"

class Chopper final :
	public Enemy
{
public:
	explicit Chopper(const std::string& chopperPath, const std::vector<std::vector<Point2f>>* levelVerticies, Player* player, const Camera* camera);
	~Chopper() override;

	virtual void Update(float& elapsedSec, const float& fpsTarget) override;
	virtual void Draw() const override;
	virtual void Reset() override;
private:
	const float m_ExplosionSpriteSize;
	const float m_Gravity;
	const std::vector<std::vector<Point2f>>* m_LevelVerticies;
	float m_DeltaTime;

	std::vector<bool> m_HasSpawned;
	std::vector<SpriteManager*> m_ExplosionSprite;

	Texture* m_pExplosionTexture;

	void DestroyChopper(const size_t& idx);
	void HandleCollision();
	void Init();
};
