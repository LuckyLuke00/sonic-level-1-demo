#include "pch.h"
#include "Motobug.h"
#include "utils.h"
#include "Camera.h"

#include <fstream>
#include <sstream>

Motobug::Motobug(const std::string& motobugPath, const std::string& minMaxPosPath, const std::vector<std::vector<Point2f>>* levelVerticies, Player* player, const Camera* camera)
	: Enemy(motobugPath, "../Resources/Sprites/Enemies/Motobug_Texture.png", Vector2f{ 39.f, 29.f }, player, camera)
	, m_IsOnGround{}
	, m_ExplosionSpriteSize{ 54.f }
	, m_MovingLeft{}
	, m_LevelVerticies{ levelVerticies }
	, m_GroundSensors{}
	, m_Gravity{ 0.21875f }
	, m_MinMaxPosPath{ minMaxPosPath }
	, m_MinMaxPositions{}
	, m_pExplosionTexture{ new Texture{"../Resources/Sprites/Enemies/Enemy_Explosion_Texture.png" } }
{
	Init();
}

Motobug::~Motobug()
{
	delete m_pExplosionTexture;

	m_pExplosionTexture = nullptr;

	for (size_t i{ 0 }; i < m_ExplosionSprite.size(); ++i)
	{
		delete m_ExplosionSprite[i];
		m_ExplosionSprite[i] = nullptr;
	}
}

void Motobug::Update(float& elapsedSec, const float& fpsTarget)
{
	Enemy::Update(elapsedSec, fpsTarget);

	for (const auto& i : m_ExplosionSprite) i->Update(elapsedSec);

	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		// Only start updating the enemy when the camera is in range
		if (utils::IsOverlapping(m_CameraRect, m_HitBox[i])) m_HasSpawned[i] = true;

		if (m_HasSpawned[i])
		{
			if (!m_IsOnGround[i])
			{
				m_Velocty[i].y -= m_Gravity * elapsedSec * fpsTarget;
			}
			// Check if the motobug should turn around
			if (m_EnemyCenters[i].x < m_MinMaxPositions[i].first)
			{
				m_pSprites[i]->Mirror();
				m_MovingLeft[i] = 1.f;
			}
			else if (m_EnemyCenters[i].x > m_MinMaxPositions[i].second)
			{
				m_MovingLeft[i] = -1.f;
			}

			if (m_MovingLeft[i] == 1.f) m_pSprites[i]->Mirror();

			m_GroundSensors[i] = Linef{ m_EnemyCenters[i], Point2f{ m_EnemyCenters[i].x, m_pSprites[i]->m_SpritePosition.y - 1.f } };

			m_Velocty[i].x = m_MovingLeft[i] * elapsedSec * fpsTarget;

			m_pSprites[i]->m_SpritePosition.x += m_Velocty[i].x;
			m_pSprites[i]->m_SpritePosition.y += m_Velocty[i].y;
		}
	}

	HandleCollision();
}

void Motobug::Draw() const
{
	Enemy::Draw();

	for (size_t i{ 0 }; i < m_ExplosionSprite.size(); ++i)
	{
		if (m_Destroyed[i]) m_ExplosionSprite[i]->Draw();
	}
}

void Motobug::Reset()
{
	Enemy::Reset();

	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		delete m_ExplosionSprite[i];
		m_ExplosionSprite[i] = nullptr;
	}

	m_ExplosionSprite.clear();
	m_GroundSensors.clear();
	m_HasSpawned.clear();
	m_IsOnGround.clear();
	m_MinMaxPositions.clear();
	m_MovingLeft.clear();

	Init();
}

void Motobug::HandleCollision()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		utils::HitInfo hitInfo{};

		for (const auto& j : *m_LevelVerticies)
		{
			if (utils::Raycast(j, m_GroundSensors[i].from, m_GroundSensors[i].to, hitInfo))
			{
				m_pSprites[i]->m_SpritePosition.y = m_HitBox[i].bottom = hitInfo.intersectPoint.y + 1.f;
				m_Velocty[i].y = 0.f;
				m_IsOnGround[i] = true;
			}
			else
			{
				m_IsOnGround[i] = false;
			}
		}
	}

	HandlePlayerCollision();
}

void Motobug::HandlePlayerCollision()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		if (utils::IsOverlapping(m_HitBox[i], m_pPlayer->m_HitBox))
		{
			if (m_pPlayer->m_PlayerProps.invincibilityShield)
			{
				DestroyMotobug(i);
			}
			else if (m_pPlayer->m_PlayerProps.invincible)
			{
				if (m_pPlayer->m_PlayerProps.isJumping)
				{
					DestroyMotobug(i);

					if (m_pPlayer->m_Velocity.y < 0.f)
					{
						m_pPlayer->m_Velocity.y = -m_pPlayer->m_Velocity.y;
					}
				}
				break;
			}
			else if (m_pPlayer->m_PlayerProps.isJumping)
			{
				if (m_pPlayer->m_Velocity.y < 0.f)
				{
					m_pPlayer->m_Velocity.y = -m_pPlayer->m_Velocity.y;
				}

				DestroyMotobug(i);
			}
			else if (CheckDeath())
			{
				m_pPlayer->m_PlayerProps.hasDied = true;
				break;
			}
			else
			{
				m_pPlayer->m_PlayerProps.isOnGround = false;
				float reboundForce{ Player::Sign(m_pPlayer->m_PlayerCenter.x - m_EnemyCenters[i].x) * 2.f };
				if (reboundForce == 0.f) reboundForce = 1.f;

				m_pPlayer->m_Velocity.x = reboundForce;
				m_pPlayer->m_Velocity.y = 4.f;

				m_pPlayer->m_PlayerProps.isHurt = true;
			}
		}
	}
}

void Motobug::Init()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		m_GroundSensors.push_back(Linef{ m_EnemyCenters[i], Point2f{ m_EnemyCenters[i].x, m_HitBox[i].bottom - 1.f } });
		m_pSprites[i]->ChangeSprite(1, 1, 1, 4, 8);
		m_IsOnGround.push_back(false);
		m_HasSpawned.push_back(false);
		m_MovingLeft.push_back(-1.f);

		m_ExplosionSprite.push_back(new SpriteManager{ m_pExplosionTexture, m_ExplosionSpriteSize, m_ExplosionSpriteSize, Point2f{ m_HitBox[i].left - (m_ExplosionSpriteSize - m_HitBox[i].width) / 2, m_HitBox[i].bottom } });
	}

	SetMinMaxPositions(m_MinMaxPosPath);
}

void Motobug::SetMinMaxPositions(const std::string& minMaxPosPath)
{
	std::ifstream file(minMaxPosPath.c_str());

	for (std::string line; std::getline(file, line); ) //read stream line by line
	{
		std::istringstream in(line);
		float left;
		float right;
		in >> left >> right;
		m_MinMaxPositions.push_back(std::make_pair(left, right));
	}
}

void Motobug::DestroyMotobug(const size_t& idx)
{
	AddScore(100);

	m_ExplosionSprite[idx]->m_SpritePosition = Point2f{ m_HitBox[idx].left - (m_ExplosionSpriteSize - m_HitBox[idx].width) / 2, m_HitBox[idx].bottom };

	SpriteManager* tempSprite{ m_ExplosionSprite[idx] };
	bool tempDestroyed{ m_Destroyed[idx] };

	m_HitBox[idx] = m_HitBox.back();
	m_ExplosionSprite[idx] = m_ExplosionSprite.back();
	m_MinMaxPositions[idx] = m_MinMaxPositions.back();
	m_Destroyed[idx] = m_Destroyed.back();
	m_HitBox.pop_back();
	m_MinMaxPositions.pop_back();

	m_ExplosionSprite.back() = tempSprite;
	m_Destroyed.back() = tempDestroyed;

	delete m_pSprites[idx];
	m_pSprites[idx] = nullptr;

	m_pSprites[idx] = m_pSprites.back();
	m_pSprites.pop_back();

	m_ExplosionSprite.back()->ChangeSprite(1, 1, 1, 14, 16, true);
	m_ExplosionSprite.back()->ChangeSpriteAfter(1, 1, 1, 1, 14, 0);

	m_Destroyed.back() = true;

	tempSprite = nullptr;

	if (!Player::m_Mute)
	{
		m_pDestroyedSound->SetVolume(Player::m_Volume);
		m_pDestroyedSound->Play(0);
	}
}
