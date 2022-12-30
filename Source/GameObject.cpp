#include "pch.h"
#include "GameObject.h"
#include <fstream>
#include <sstream>
#include "SoundEffect.h"
#include "SpriteManager.h"
#include "Texture.h"
#include "utils.h"
#include "Player.h"

GameObject::GameObject(const std::vector<Point2f>& locations, const std::string& texturePath,
	const Vector2f& hitboxSize)
	: GameObject{ locations, texturePath, hitboxSize, hitboxSize.x, hitboxSize.y }
{
}

GameObject::GameObject(const std::string& locationFilePath, const std::string& texturePath, const Vector2f& hitboxSize,
	float spriteWidth, float spriteHeight)
	: m_pTexture{ new Texture{texturePath} }
	, m_SpriteHeight{ spriteHeight }
	, m_SpriteWidth{ spriteWidth }
	, m_HitBoxSize{ hitboxSize }
	, m_DeltaTime{ 0.0f }
{
	GetPositionsFromFile(locationFilePath);
	InitGameObjects();
}

GameObject::GameObject(const std::vector<Point2f>& locations, const std::string& texturePath,
	const Vector2f& hitboxSize, float spriteWidth, float spriteHeight)
	: m_pTexture{ new Texture{texturePath} }
	, m_SpriteHeight{ spriteHeight }
	, m_SpriteWidth{ spriteWidth }
	, m_HitBoxSize{ hitboxSize }
	, m_DeltaTime{ 0.0f }
	, m_Locations{ locations }
{
	InitGameObjects();
}

GameObject::~GameObject()
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		delete m_pSprites[i];
		m_pSprites[i] = nullptr;
	}

	delete m_pTexture;
	m_pTexture = nullptr;
}

void GameObject::Draw() const
{
	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		m_pSprites[i]->Draw();
	}
	if (Player::m_Debug)
	{
		utils::SetColor(Color4f{ 0.0f, 1.0f, 1.0f, 1.0f });
		for (const auto& i : m_HitBox) utils::DrawRect(i);
	}
}

void GameObject::Update(float& elapsedSec, const float& fpsTarget)
{
	m_DeltaTime = elapsedSec * fpsTarget;
	for (const auto& i : m_pSprites)
	{
		i->Update(elapsedSec);
	}
}

void GameObject::Reset()
{
	// Only if the sprites size is not equal to the location size - reset, otherwise it wil loop indefinitely
	if (m_pSprites.size() != m_Locations.size())
	{
		for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
		{
			delete m_pSprites[i];
			m_pSprites[i] = nullptr;
		}
		m_HitBox.clear();
		m_pSprites.clear();

		InitGameObjects();
	}

	for (size_t i{ 0 }; i < m_pSprites.size(); ++i)
	{
		m_pSprites[i]->m_SpritePosition = m_Locations[i];
	}
}

std::vector<Point2f> GameObject::GetPositions() const
{
	return m_Locations;
}

std::vector<Point2f> GameObject::GetPositionsFromFile(const std::string& locationFilePath)
{
	std::ifstream file(locationFilePath.c_str());

	for (std::string line; std::getline(file, line);) //read stream line by line
	{
		std::istringstream in(line);
		Point2f a;
		in >> a.x >> a.y;
		m_Locations.push_back(a);
	}
	return m_Locations;
}

void GameObject::InitGameObjects()
{
	for (size_t i{ 0 }; i < m_Locations.size(); ++i)
	{
		m_pSprites.push_back(new SpriteManager{ m_pTexture, m_SpriteWidth, m_SpriteHeight, m_Locations[i] });
		if (m_SpriteWidth != m_HitBoxSize.x)
			// If the hit box width is not the same as the sprite width draw the hit box around the sprites center
		{
			m_HitBox.push_back(Rectf{
				m_Locations[i].x + m_SpriteWidth / 2 - m_HitBoxSize.x / 2,
				m_Locations[i].y + m_SpriteHeight / 2 - m_HitBoxSize.y / 2, m_HitBoxSize.x, m_HitBoxSize.y
				});
		}
		else m_HitBox.push_back(Rectf{ m_Locations[i].x, m_Locations[i].y, m_HitBoxSize.x, m_HitBoxSize.y });
	}
}
