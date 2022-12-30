#include "pch.h"
#include "Chopper.h"
#include "utils.h"

Chopper::Chopper(const std::string& chopperPath, const std::vector<std::vector<Point2f>>* levelVerticies, Player* player, const Camera* camera)
	: Enemy(chopperPath, "../Resources/Sprites/Enemies/Chopper_Texture.png", Vector2f{ 30.f, 32.f }, player, camera)
	, m_ExplosionSpriteSize{ 54.f }
	, m_LevelVerticies{ levelVerticies }
	, m_Gravity{ 0.09375f }
	, m_pExplosionTexture{ new Texture{"../Resources/Sprites/Enemies/Enemy_Explosion_Texture.png" } }
	, m_DeltaTime{ 0.f }
{
	Init();
}

Chopper::~Chopper()
{
	delete m_pExplosionTexture;

	m_pExplosionTexture = nullptr;

	for (size_t i{ 0 }; i < m_ExplosionSprite.size(); ++i)
	{
		delete m_ExplosionSprite[i];
		m_ExplosionSprite[i] = nullptr;
	}
}

void Chopper::Update(float& elapsedSec, const float& fpsTarget)
{
	Enemy::Update(elapsedSec, fpsTarget);

	m_DeltaTime = elapsedSec * fpsTarget;

	for (const auto& i : m_ExplosionSprite) i->Update(elapsedSec);

	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		// Only start updating the enemy when the camera is in range
		if (!m_HasSpawned[i] && m_CameraRect.left + m_CameraRect.width >= m_pSprites[i]->m_SpritePosition.x) m_HasSpawned[i] = true;

		if (m_HasSpawned[i])
		{
			m_Velocty[i].y -= m_Gravity * m_DeltaTime;

			m_pSprites[i]->m_SpritePosition.y += m_Velocty[i].y * m_DeltaTime;
			m_HitBox[i].bottom = m_pSprites[i]->m_SpritePosition.y;

			if (m_pSprites[i]->m_SpritePosition.y <= m_Locations[i].y)
			{
				m_Velocty[i].y = 7.f;
			}
		}
		else
		{
			m_pSprites[i]->m_SpritePosition.y = m_Locations[i].y;
		}
	}

	HandleCollision();
}

void Chopper::Draw() const
{
	Enemy::Draw();

	for (size_t i{ 0 }; i < m_ExplosionSprite.size(); ++i)
	{
		if (m_Destroyed[i]) m_ExplosionSprite[i]->Draw();
	}
}

void Chopper::Reset()
{
	Enemy::Reset();

	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		delete m_ExplosionSprite[i];
		m_ExplosionSprite[i] = nullptr;
	}

	m_ExplosionSprite.clear();
	m_HasSpawned.clear();

	Init();
}

void Chopper::DestroyChopper(const size_t& idx)
{
	AddScore(100);

	m_ExplosionSprite[idx]->m_SpritePosition = Point2f{ m_HitBox[idx].left - (m_ExplosionSpriteSize - m_HitBox[idx].width) / 2, m_HitBox[idx].bottom };

	SpriteManager* tempSprite{ m_ExplosionSprite[idx] };
	bool tempDestroyed{ m_Destroyed[idx] };

	m_HitBox[idx] = m_HitBox.back();
	m_ExplosionSprite[idx] = m_ExplosionSprite.back();
	m_Destroyed[idx] = m_Destroyed.back();
	m_HitBox.pop_back();

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

void Chopper::HandleCollision()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		if (utils::IsOverlapping(m_HitBox[i], m_pPlayer->m_HitBox))
		{
			if (m_pPlayer->m_PlayerProps.invincibilityShield)
			{
				DestroyChopper(i);
			}
			else if (m_pPlayer->m_PlayerProps.invincible)
			{
				if (m_pPlayer->m_PlayerProps.isJumping)
				{
					DestroyChopper(i);

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

				DestroyChopper(i);
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

void Chopper::Init()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		m_pSprites[i]->ChangeSprite(1, 1, 1, 5, 8);
		m_HasSpawned.push_back(false);

		m_ExplosionSprite.push_back(new SpriteManager{ m_pExplosionTexture, m_ExplosionSpriteSize, m_ExplosionSpriteSize, Point2f{ m_HitBox[i].left - (m_ExplosionSpriteSize - m_HitBox[i].width) / 2, m_HitBox[i].bottom } });
	}
}
