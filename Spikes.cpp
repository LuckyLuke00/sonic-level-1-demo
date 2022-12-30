#include "pch.h"
#include "Spikes.h"

#include "Player.h"
#include "SoundEffect.h"
#include "utils.h"

Spikes::Spikes(const std::string& BigSpikesLocationFilePath, const std::string& SmallSpikesLocationFilePath, Player* player)
	: GameObject{ BigSpikesLocationFilePath, "./Resources/Sprites/Game Objects/Big_Spikes_Texture.png", Vector2f{ 10.0f, 32.0f }, 10.0f, 32.0f }
	, m_SmallSpikes{ SmallSpikesLocationFilePath, "./Resources/Sprites/Game Objects/Small_Spikes_Texture.png", Vector2f{ 6.0f, 32.0f }, 6.0f, 32.0f }
	, m_pPlayer{ player }
	, m_SpikeDeathSound{ new SoundEffect{ "./Resources/Sound Effects/Spike.wav" } }
{
	m_SpikeDeathSound->SetVolume(Player::m_Volume);
}

Spikes::~Spikes()
{
	delete m_SpikeDeathSound;

	m_pPlayer = nullptr;
	m_SpikeDeathSound = nullptr;
}

void Spikes::Draw() const
{
	GameObject::Draw();
	m_SmallSpikes.Draw();
}

void Spikes::Update(float& elapsedSec, const float& fpsTarget)
{
	GameObject::Update(elapsedSec, fpsTarget);
	m_SmallSpikes.Update(elapsedSec, fpsTarget);
}

bool Spikes::HandleCollision()
{
	for (const auto& i : m_HitBox)
	{
		if (utils::IsOverlapping(i, m_pPlayer->m_HitBox))
		{
			// Make sure the player is only hurt when on top of the spikes
			if (m_pPlayer->m_HitBox.bottom < i.bottom + i.height && m_pPlayer->m_Velocity.y < 0.0f && m_pPlayer->m_HitBox.bottom > i.bottom + i.height / 2)
			{
				if (!m_pPlayer->m_PlayerProps.invincible)
				{
					// Calculate how the player should rebound
					m_pPlayer->m_Velocity.y = 4.0f;
					m_pPlayer->m_Velocity.x = 2.0f * Player::Sign(m_pPlayer->m_PlayerCenter.x - i.left);
					m_pPlayer->m_PlayerProps.isHurt = true;

					CheckDeath(); // Check if collision was fatal
				}
				else
				{
					m_pPlayer->m_PlayerProps.isOnGround = true;
					m_pPlayer->m_PlayerProps.canJump = true; // This is here because when it gets wrapped to the calculated position there is a problem that you can't jump
					m_pPlayer->m_Velocity.y = 0.0f;
					m_pPlayer->m_PlayerPos.y = i.bottom + i.height - m_pPlayer->m_HitBoxOffset.y;
				}
			}
			// Check if the player is pushing against the hazardous object - This is a bit complicated but it works - The reason it is complicated is because the player should not be hurt when pushing against it
			if (m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width > i.left && m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width < i.left + i.width && m_pPlayer->m_PlayerProps.isOnGround && m_pPlayer->m_PlayerPos.y < i.bottom + i.height / 2)
			{
				m_pPlayer->m_PlayerPos.x -= m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width - i.left;
				m_pPlayer->m_Velocity.x = 0.0f;
			}
			else if (m_pPlayer->m_HitBox.left < i.left + i.width && m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width > i.left + i.width && m_pPlayer->m_PlayerProps.isOnGround && m_pPlayer->m_PlayerPos.y < i.bottom + i.height / 2)
			{
				m_pPlayer->m_PlayerPos.x += i.left + i.width - m_pPlayer->m_HitBox.left;
				m_pPlayer->m_Velocity.x = 0.0f;
			}
			return true;
		}
	}
	return false;
}

void Spikes::CheckDeath() const
{
	if (m_pPlayer->m_PlayerProps.rings == 0)
	{
		m_pPlayer->m_PlayerProps.spikeDeath = true;
		m_pPlayer->m_PlayerProps.hasDied = true;
		if (!Player::m_Mute)
		{
			m_SpikeDeathSound->SetVolume(Player::m_Volume);
			m_SpikeDeathSound->Play(0);
		}
	}
}
