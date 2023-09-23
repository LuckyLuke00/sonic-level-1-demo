#include "pch.h"
#include "Checkpoint.h"

#include "Player.h"
#include "SoundEffect.h"
#include "utils.h"

Checkpoint::Checkpoint(const std::vector<Point2f>& locations, Player* player)
	: GameObject{ locations, "./Resources/Sprites/Game Objects/Checkpoint_Texture.png", Vector2f{ 16.0f, 64.0f }, 40.0f, 64.0f }
	, m_Player{ player }
	, m_pCheckpointSound{ new SoundEffect{"./Resources/Sound Effects/StarPost.wav"} }
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i) m_CheckpointHit.push_back(false);
}

Checkpoint::~Checkpoint()
{
	delete m_pCheckpointSound;
	m_pCheckpointSound = nullptr;

	m_Player = nullptr;
}

void Checkpoint::HandleCollision()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		if (utils::IsOverlapping(m_HitBox[i], m_Player->m_HitBox) && m_CheckpointHit[i] == false)
		{
			if (!Player::m_Mute)
			{
				m_pCheckpointSound->SetVolume(Player::m_Volume);
				m_pCheckpointSound->Play(0);
			}

			float newSpeed{ floorf(abs(m_Player->m_Velocity.x)) };
			if (newSpeed < 1) newSpeed = 1;
			if (newSpeed > 6) newSpeed = 6;

			m_pSprites[i]->ChangeSprite(2, 1, 1, 32, 32 * newSpeed); // We multiply the fps so the amount of loops is always completed in one second
			if (m_HitBox[i].left < m_Player->m_HitBox.left) m_pSprites[i]->Reverse(); // If the player comes from the right reverse the animation
			m_pSprites[i]->ChangeSpriteAfter(static_cast<int>(newSpeed), 1, 1, 2, 2, 16); // Loop animation (amount depends on xSpeed) and change it to the end animation

			m_CheckpointHit[i] = true;

			if (m_Player->m_RespawnPoint.x < m_pSprites[i]->m_SpritePosition.x)
			{
				m_Player->m_RespawnPoint = m_pSprites[i]->m_SpritePosition;
			}
		}
	}
}

void Checkpoint::Reset()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		m_CheckpointHit[i] = false;
		m_pSprites[i]->ChangeSprite(1, 1, 1, 1, 32);
	}
}
