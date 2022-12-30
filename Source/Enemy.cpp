#include "pch.h"
#include "Enemy.h"

Enemy::Enemy(const std::string& startPosPath, const std::string& texturePath, const Vector2f& hitBoxSize, Player* player, const Camera* camera)
	: GameObject{ startPosPath, texturePath, hitBoxSize, hitBoxSize.x, hitBoxSize.y }
	, m_pPlayer{ player }
	, m_pDestroyedSound{ new SoundEffect{ "../Resources/Sound Effects/Destroy.wav" } }
	, m_EnemyCenters{}
	, m_Velocty{}
	, m_CameraRect{ camera->GetCameraRect() }
	, m_pCamera{ camera }
{
	Init();
}

Enemy::~Enemy()
{
	delete m_pDestroyedSound;
	m_pDestroyedSound = nullptr;
}

void Enemy::Update(float& elapsedSec, const float& fpsTarget)
{
	GameObject::Update(elapsedSec, fpsTarget);
	m_CameraRect = m_pCamera->GetCameraRect();

	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		m_HitBox[i].left = m_pSprites[i]->m_SpritePosition.x;
		m_EnemyCenters[i] = Point2f{ m_pSprites[i]->m_SpritePosition.x + m_HitBoxSize.x / 2, m_pSprites[i]->m_SpritePosition.y + m_HitBoxSize.y / 2 };
	}
}

void Enemy::Reset()
{
	GameObject::Reset();

	m_Destroyed.clear();
	m_EnemyCenters.clear();
	m_Velocty.clear();

	Init();
}

bool Enemy::CheckDeath()
{
	return m_pPlayer->m_PlayerProps.rings == 0;
}

void Enemy::AddScore(const int score)
{
	m_pPlayer->m_PlayerProps.score += score;
}

void Enemy::Init()
{
	for (const auto& i : m_pSprites)
	{
		m_EnemyCenters.push_back(Point2f{ i->m_SpritePosition.x + m_HitBoxSize.x / 2, i->m_SpritePosition.y + m_HitBoxSize.y / 2 });
		m_Velocty.push_back(Vector2f{ 0.f, 0.f });
		m_Destroyed.push_back(false);
	}
}
