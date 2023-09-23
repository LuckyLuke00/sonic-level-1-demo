#include "pch.h"
#include "ItemBox.h"

#include <fstream>
#include <sstream>

#include "Player.h"
#include "SoundEffect.h"
#include "utils.h"
#include "Rings.h"

ItemBox::ItemBox(const std::string& ItemBoxPath, Player* player, Rings* rings)
	: GameObject(ItemBoxPath, "./Resources/Sprites/Game Objects/Item Box/Item_Box_Texture.png", Vector2f{ 32.0f, 32.0f }, 32.0f, 32.0f)
	, m_pPlayer{ player }
	, m_pBoxDestroyedSound{ new SoundEffect{ "./Resources/Sound Effects/Destroy.wav" } }
	, m_pExplosionTexture{ new Texture{"./Resources/Sprites/Game Objects/Item Box/Item_Box_Explosion.png" } }
	, m_pItemBoxItemsTexture{ new Texture{ "./Resources/Sprites/Game Objects/Item Box/Item_Box_Items.png" } }
	, m_pRings{ rings }
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		m_ExplosionSprite.push_back(new SpriteManager{ m_pExplosionTexture, 48.0f, 48.0f, Point2f{ m_HitBox[i].left - (48.0f - m_HitBox[i].width) / 2, m_HitBox[i].bottom } });
		m_Destroyed.push_back(false);
	}
}

ItemBox::~ItemBox()
{
	delete m_pExplosionTexture;
	delete m_pItemBoxItemsTexture;
	delete m_pBoxDestroyedSound;

	m_pExplosionTexture = nullptr;
	m_pItemBoxItemsTexture = nullptr;
	m_pBoxDestroyedSound = nullptr;

	for (size_t i{ 0 }; i < m_ItemRingSprite.size(); ++i)
	{
		delete m_ItemRingSprite[i];
		m_ItemRingSprite[i] = nullptr;
	}
	for (size_t i{ 0 }; i < m_ItemLifeSprite.size(); ++i)
	{
		delete m_ItemLifeSprite[i];
		m_ItemLifeSprite[i] = nullptr;
	}
	for (size_t i{ 0 }; i < m_ExplosionSprite.size(); ++i)
	{
		delete m_ExplosionSprite[i];
		m_ExplosionSprite[i] = nullptr;
	}
	for (size_t i{ 0 }; i < m_ItemInvincibilitySprite.size(); ++i)
	{
		delete m_ItemInvincibilitySprite[i];
		m_ItemInvincibilitySprite[i] = nullptr;
	}
}

void ItemBox::Draw() const
{
	GameObject::Draw();
	for (size_t i{ 0 }; i < m_ExplosionSprite.size(); ++i)
	{
		if (m_Destroyed[i]) m_ExplosionSprite[i]->Draw();
	}
	for (const auto& i : m_ItemInvincibilitySprite) i->Draw();
	for (const auto& i : m_ItemRingSprite) i->Draw();
	for (const auto& i : m_ItemLifeSprite) i->Draw();
}

void ItemBox::Update(float& elapsedSec, const float& fpsTarget)
{
	GameObject::Update(elapsedSec, fpsTarget);
	for (const auto& i : m_ExplosionSprite) i->Update(elapsedSec);
	for (const auto& i : m_ItemInvincibilitySprite) i->Update(elapsedSec);
	for (const auto& i : m_ItemLifeSprite) i->Update(elapsedSec);
	for (const auto& i : m_ItemRingSprite) i->Update(elapsedSec);
	HandleCollision();
}

bool ItemBox::HandleCollision()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		if (utils::IsOverlapping(m_HitBox[i], m_pPlayer->m_HitBox) && !m_Destroyed[i])
		{
			// Make sure the item box is only destroyed when jumping on top
			if (m_pPlayer->m_HitBox.bottom < m_HitBox[i].bottom + m_HitBox[i].height && m_pPlayer->m_Velocity.y < 0.0f && m_pPlayer->m_HitBox.bottom > m_HitBox[i].bottom + m_HitBox[i].height / 2)
			{
				if (m_pPlayer->m_PlayerProps.isJumping)
				{
					// Calculate how the player should rebound
					m_pPlayer->m_Velocity.y = 4.0f;

					if (!Player::m_Mute)
					{
						m_pBoxDestroyedSound->SetVolume(Player::m_Volume);
						m_pBoxDestroyedSound->Play(0);
					}

					m_pSprites[i]->ChangeSprite(1, 1, 2, 1, 32);
					m_ExplosionSprite[i]->ChangeSprite(1, 1, 1, 14, 16, true);
					m_ExplosionSprite[i]->ChangeSpriteAfter(1, 1, 1, 1, 14, 0);

					for (const auto& j : m_ItemRingSprite) // Check if ring item is in the itembox
					{
						if (utils::IsPointInRect(j->m_SpritePosition, m_HitBox[i]))
						{
							j->ChangeSprite(1, 1, 1, 4, 0);
							m_pRings->AddRings(10);
						}
					}
					for (const auto& k : m_ItemLifeSprite) // Check if ring item is in the itembox
					{
						if (utils::IsPointInRect(k->m_SpritePosition, m_HitBox[i]))
						{
							k->ChangeSprite(2, 1, 1, 4, 0);
							m_pPlayer->m_PlayerProps.extraLife = true;
						}
					}
					for (const auto& l : m_ItemInvincibilitySprite) // Check if ring item is in the itembox
					{
						if (utils::IsPointInRect(l->m_SpritePosition, m_HitBox[i]))
						{
							l->ChangeSprite(3, 1, 1, 4, 0);
							m_pPlayer->m_PlayerProps.invincibilityShield = true;
						}
					}

					m_Destroyed[i] = true;
				}
				else
				{
					m_pPlayer->m_PlayerProps.isOnGround = true;
					m_pPlayer->m_PlayerProps.canJump = true; // This is here because when it gets wrapped to the calculated position there is a problem that you can't jump
					m_pPlayer->m_Velocity.y = 0.0f;
					m_pPlayer->m_PlayerPos.y = m_HitBox[i].bottom + m_HitBox[i].height - m_pPlayer->m_HitBoxOffset.y;
				}
			}
			else if (m_pPlayer->m_HitBox.bottom + m_pPlayer->m_HitBox.height > m_HitBox[i].bottom && m_pPlayer->m_HitBox.bottom + m_pPlayer->m_HitBox.height < m_HitBox[i].bottom + m_HitBox[i].height) // If player is hitting the box from below
			{
				m_pPlayer->m_Velocity.y = -4.0f;
			}
			// Check if the player is pushing against the object - This is a bit complicated but it works
			if (m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width > m_HitBox[i].left && m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width < m_HitBox[i].left + m_HitBox[i].width && m_pPlayer->m_PlayerProps.isOnGround && m_pPlayer->m_PlayerPos.y < m_HitBox[i].bottom + m_HitBox[i].height / 2)
			{
				m_pPlayer->m_PlayerPos.x -= m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width - m_HitBox[i].left;
				m_pPlayer->m_Velocity.x = 0.0f;
			}
			else if (m_pPlayer->m_HitBox.left <= m_HitBox[i].left + m_HitBox[i].width && m_pPlayer->m_HitBox.left + m_pPlayer->m_HitBox.width > m_HitBox[i].left + m_HitBox[i].width && m_pPlayer->m_PlayerProps.isOnGround && m_pPlayer->m_PlayerPos.y < m_HitBox[i].bottom + m_HitBox[i].height / 2)
			{
				m_pPlayer->m_PlayerPos.x += m_HitBox[i].left + m_HitBox[i].width - m_pPlayer->m_HitBox.left;
				m_pPlayer->m_Velocity.x = 0.0f;
			}
			return true;
		}
	}
	return false;
}

void ItemBox::Reset()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		m_Destroyed[i] = false;
		m_ExplosionSprite[i]->ChangeSprite(1, 1, 1, 14, 16, true);
		m_pSprites[i]->ChangeSprite(1, 1, 1, 1, 32);
	}
	for (const auto& i : m_ItemRingSprite) i->ChangeSprite(1, 1, 1, 4, 16);
	for (const auto& i : m_ItemLifeSprite) i->ChangeSprite(2, 1, 1, 4, 16);
	for (const auto& i : m_ItemInvincibilitySprite) i->ChangeSprite(3, 1, 1, 4, 16);
}

void ItemBox::AddRingBoxLocations(const std::string& locationPath)
{
	m_ItemBoxRingLocations = Initialize(locationPath);

	for (size_t i{ 0 }; i < m_ItemBoxRingLocations.size(); ++i)
	{
		m_ItemRingSprite.push_back(new SpriteManager{ m_pItemBoxItemsTexture, 16.0f, 16.0f, m_ItemBoxRingLocations[i] });
		m_ItemRingSprite[i]->ChangeSprite(1, 1, 1, 4, 16);
	}
}

void ItemBox::AddInvincibilityBoxLocations(const std::string& locationPath)
{
	m_ItemBoxInvincibilityLocations = Initialize(locationPath);

	for (size_t i{ 0 }; i < m_ItemBoxInvincibilityLocations.size(); ++i)
	{
		m_ItemInvincibilitySprite.push_back(new SpriteManager{ m_pItemBoxItemsTexture, 16.0f, 16.0f, m_ItemBoxInvincibilityLocations[i] });
		m_ItemInvincibilitySprite[i]->ChangeSprite(3, 1, 1, 4, 16);
	}
}

void ItemBox::AddLifeBoxLocations(const std::string& locationPath)
{
	m_ItemBoxLifeLocations = Initialize(locationPath);

	for (size_t i{ 0 }; i < m_ItemBoxLifeLocations.size(); ++i)
	{
		m_ItemLifeSprite.push_back(new SpriteManager{ m_pItemBoxItemsTexture, 16.0f, 16.0f, m_ItemBoxLifeLocations[i] });
		m_ItemLifeSprite[i]->ChangeSprite(2, 1, 1, 4, 16);
	}
}

std::vector<Point2f> ItemBox::Initialize(const std::string& spriteLocationsPath) const
{
	std::vector<Point2f> locationsFromFile{};

	std::ifstream file(spriteLocationsPath.c_str());

	for (std::string line; std::getline(file, line); ) //read stream line by line
	{
		std::istringstream in(line);
		Point2f a;
		in >> a.x >> a.y;
		locationsFromFile.push_back(a);
	}

	return locationsFromFile;
}
