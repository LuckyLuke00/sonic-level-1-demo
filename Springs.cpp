#include "pch.h"
#include "Springs.h"

#include "Player.h"
#include "SoundEffect.h"
#include "utils.h"

Springs::Springs(const std::string& SpringLocationsPath, Player* player)
	: GameObject{ SpringLocationsPath, "./Resources/Sprites/Game Objects/Spring_Yellow_Texture.png", Vector2f{ 32.0f, 16.0f }, 32.0f, 32.0f }
	, m_pPlayer{ player }
	, m_pSpringSound{ new SoundEffect{"./Resources/Sound Effects/Spring.wav" } }
{
	m_pSpringSound->SetVolume(Player::m_Volume);
}

Springs::~Springs()
{
	delete m_pSpringSound;
	m_pSpringSound = nullptr;
}

bool Springs::HandleCollision()
{
	for (size_t i{ 0 }; i < m_HitBox.size(); ++i)
	{
		if (utils::IsOverlapping(m_HitBox[i], m_pPlayer->m_HitBox))
		{
			// Check top collision
			if (m_pPlayer->m_HitBox.bottom < m_HitBox[i].bottom + m_HitBox[i].height && m_pPlayer->m_Velocity.y < 0.0f && m_pPlayer->m_HitBox.bottom > m_HitBox[i].bottom + m_HitBox[i].height / 2)
			{
				m_pPlayer->m_PlayerProps.isJumping = false;
				m_pPlayer->m_PlayerProps.isOnGround = true;
				m_pPlayer->m_PlayerProps.Spring = true;
				m_pPlayer->m_Velocity.y = 10.0f;
				m_pSprites[i]->ChangeSprite(1, 1, 1, 8, 16);
				m_pSprites[i]->ChangeSpriteAfter(1, 1, 1, 1, 1, 16);

				if (!Player::m_Mute)
				{
					m_pSpringSound->SetVolume(Player::m_Volume);
					m_pSpringSound->Play(0);
				}
			}
			// Check if the player is pushing against the object
			if (m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width > m_HitBox[i].left && m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width < m_HitBox[i].left + m_HitBox[i].width && m_pPlayer->m_PlayerProps.isOnGround && m_pPlayer->m_PlayerPos.y < m_HitBox[i].bottom + m_HitBox[i].height / 2)
			{
				m_pPlayer->m_PlayerPos.x -= m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width - m_HitBox[i].left;
				//m_pPlayer->m_Velocity.x = 0.0f;
				//m_pPlayer->m_PlayerProps.isPushing = true;
				return true;
			}
			if (m_pPlayer->m_HitBox.left < m_HitBox[i].left + m_HitBox[i].width && m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width > m_HitBox[i].left + m_HitBox[i].width && m_pPlayer->m_PlayerProps.isOnGround && m_pPlayer->m_PlayerPos.y < m_HitBox[i].bottom + m_HitBox[i].height / 2)
			{
				m_pPlayer->m_PlayerPos.x += m_HitBox[i].left + m_HitBox[i].width - m_pPlayer->m_HitBox.left;
				//m_pPlayer->m_Velocity.x = 0.0f;
				//m_pPlayer->m_PlayerProps.isPushing = true;
				return true;
			}
		}
	}
	return false;
}
