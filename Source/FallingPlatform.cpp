#include "pch.h"
#include "FallingPlatform.h"

#include "Player.h"
#include "utils.h"

FallingPlatform::FallingPlatform(const std::string& startPosPath, Player* player)
	: GameObject{ startPosPath, "../Resources/Sprites/Game Objects/Platform_Texture.png", Vector2f{ 64.0f, 29.0f }, 64.0f, 29.0f }
	, m_FallingPlatformBottomPosY{ 0.0f }
	, m_FallingSpeed{ 7.0f }
	, m_FallTime{ 0.5f }
	, m_ResetTime{ 3.0f }
	, m_pPlayer{ player }
{
	for (size_t i{ 0 }; i < m_HitBox.size(); ++i)
	{
		m_FallTimer.push_back(m_FallTime);
		m_ResetTimer.push_back(m_ResetTime);
		m_StartTimer.push_back(false);
	}
}

bool FallingPlatform::HandleCollision()
{
	for (size_t i{ 0 }; i < m_HitBox.size(); ++i)
	{
		if (utils::IsOverlapping(m_HitBox[i], m_pPlayer->m_PlayerShape) && m_FallTimer[i] > 0.0f)
		{
			if (m_pPlayer->m_PlayerShape.bottom < m_HitBox[i].bottom + m_HitBox[i].height && m_pPlayer->m_Velocity.y < 0.0f && m_pPlayer->m_PlayerShape.bottom > m_HitBox[i].bottom + m_HitBox[i].height / 2)
			{
				m_pPlayer->m_PlayerProps.isOnGround = true;
				m_pPlayer->m_PlayerProps.canJump = true; // This is here because when it gets wrapped to the calculated position there is a problem that you can't jump
				m_pPlayer->m_Velocity.y = 0.0f;
				m_pPlayer->m_PlayerPos.y = m_HitBox[i].bottom + m_HitBox[i].height - m_pPlayer->m_HitBoxOffset.y;
				m_StartTimer[i] = true;
			}
			return true;
		}
	}
	return false;
}

void FallingPlatform::Update(float& elapsedSec, const float& fpsTarget)
{
	GameObject::Update(elapsedSec, fpsTarget);

	for (size_t i{ 0 }; i < m_HitBox.size(); ++i)
	{
		if (m_StartTimer[i] == true)
		{
			m_FallTimer[i] -= elapsedSec;

			if (m_FallTimer[i] <= 0.0f)
			{
				m_pSprites[i]->m_SpritePosition.y -= m_FallingSpeed * m_DeltaTime;
				m_HitBox[i].bottom -= m_FallingSpeed * m_DeltaTime;

				if (m_FallingPlatformBottomPosY >= m_HitBox[i].bottom)
				{
					m_ResetTimer[i] -= elapsedSec;

					if (m_ResetTimer[i] <= 0.0f)
					{
						m_ResetTimer[i] = m_ResetTime;
						m_FallTimer[i] = m_FallTime;

						m_StartTimer[i] = false;

						m_pSprites[i]->m_SpritePosition.y = m_Locations[i].y;
						m_HitBox[i].bottom = m_Locations[i].y + m_SpriteHeight / 2 - m_HitBoxSize.y / 2;
					}
				}
			}
		}
	}
}

void FallingPlatform::Reset()
{
	for (size_t i{ 0 }; i < m_HitBox.size(); ++i)
	{
		m_ResetTimer[i] = m_ResetTime;
		m_FallTimer[i] = m_FallTime;

		m_StartTimer[i] = false;

		m_pSprites[i]->m_SpritePosition.y = m_Locations[i].y;
		m_HitBox[i].bottom = m_Locations[i].y + m_SpriteHeight / 2 - m_HitBoxSize.y / 2;
	}
}
