#include "pch.h"
#include "Rings.h"

#include "Player.h"
#include "SoundEffect.h"
#include "utils.h"

Rings::Rings(const std::string& locationFilePath, Player* player)
	: GameObject{ locationFilePath, "./Resources/Sprites/Game Objects/Ring_Texture.png", Vector2f{ 13.0f, 13.0f}, 16.0f, 16.f }
	, m_HasBeenAwarded{ false }
	, m_RingBonusAmount{ 100 }
	, m_pPlayer{ player }
	, m_pRingLossSound{ new SoundEffect{ "./Resources/Sound Effects/LoseRings.wav"} }
	, m_pRingSound{ new SoundEffect{ "./Resources/Sound Effects/Ring.wav" } }
{
	for (const auto& i : m_pSprites) i->ChangeSprite(1, 1, 1, 16, 32); // Set animation of rings
	m_pRingSound->SetVolume(Player::m_Volume - 25);
}

Rings::~Rings()
{
	delete m_pRingLossSound;
	delete m_pRingSound;

	m_pPlayer = nullptr;
	m_pRingLossSound = nullptr;
	m_pRingSound = nullptr;
}

void Rings::HandleCollision()
{
	for (size_t i{ 0 }; i < m_HitBox.size(); ++i)
	{
		if (utils::IsOverlapping(m_pPlayer->m_HitBox, m_HitBox[i]))
		{
			if (!Player::m_Mute) m_pRingSound->Play(0);

			m_HitBox[i] = m_HitBox.back();
			m_HitBox.pop_back();

			delete m_pSprites[i];
			m_pSprites[i] = nullptr;

			m_pSprites[i] = m_pSprites.back();
			m_pSprites.pop_back();

			++m_pPlayer->m_PlayerProps.rings;
		}
	}
}

void Rings::Update(float& elapsedSec, const float& fpsTarget)
{
	GameObject::Update(elapsedSec, fpsTarget);
	m_pRingLossSound->SetVolume(Player::m_Volume);
	m_pRingSound->SetVolume(Player::m_Volume - 25);

	if (m_pPlayer->m_PlayerProps.isHurt && m_pPlayer->m_PlayerProps.rings != 0)
	{
		m_pPlayer->m_PlayerProps.rings = 0;
		if (!Player::m_Mute) m_pRingLossSound->Play(0);
	}
	if (AwardBonus()) m_pPlayer->m_PlayerProps.extraLife = true;
}

void Rings::Reset()
{
	GameObject::Reset();

	m_pPlayer->m_PlayerProps.rings = 0;

	for (const auto& i : m_pSprites) i->ChangeSprite(1, 1, 1, 16, 32); // Set animation of rings
}

void Rings::AddRings(const int& ringsToAdd)
{
	m_pPlayer->m_PlayerProps.rings += ringsToAdd;
	if (!Player::m_Mute) m_pRingSound->Play(0);
}

bool Rings::AwardBonus()
{
	if (m_pPlayer->m_PlayerProps.rings != 0 && m_pPlayer->m_PlayerProps.rings / 100 >= 1 && !m_HasBeenAwarded)
	{
		m_HasBeenAwarded = true;
		return true;
	}
	if (m_pPlayer->m_PlayerProps.rings / 100 < 1) m_HasBeenAwarded = false;

	return false;
}
