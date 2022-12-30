#pragma once
#include "GameObject.h"
#include "SoundEffect.h"
#include "Camera.h"
#include "Player.h"

class Enemy :
	public GameObject
{
public:
	explicit Enemy(const std::string& startPosPath, const std::string& texturePath, const Vector2f& hitBoxSize, Player* player, const Camera* camera);
	virtual ~Enemy();

	virtual void Update(float& elapsedSec, const float& fpsTarget);
	virtual void Reset();
protected:
	Player* m_pPlayer;

	const Camera* m_pCamera;
	SoundEffect* m_pDestroyedSound;

	Rectf m_CameraRect;

	std::vector<bool> m_Destroyed;
	std::vector<Point2f> m_EnemyCenters;
	std::vector<Vector2f> m_Velocty;

	bool CheckDeath();
	void AddScore(const int score);
	void Init();
};
