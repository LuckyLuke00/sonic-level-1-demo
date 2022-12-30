#include "pch.h"
#include "MovingPlatform.h"

#include <fstream>
#include <sstream>

#include "Player.h"
#include "utils.h"

MovingPlatform::MovingPlatform(const std::string& startPosPath, const std::string& minMaxPosPath, Player* player)
	: GameObject{ startPosPath, "../Resources/Sprites/Game Objects/Platform_Texture.png", Vector2f{ 64.0f, 29.0f }, 64.0f, 29.0f }
	, m_Speed{ 1.0f }
	, m_pPlayer{ player }
{
	Initialize(minMaxPosPath);
}

bool MovingPlatform::HandleCollision()
{
	for (size_t i{ 0 }; i < m_HitBox.size(); ++i)
	{
		if (utils::IsOverlapping(m_HitBox[i], m_pPlayer->m_PlayerShape))
		{
			if (m_pPlayer->m_PlayerShape.bottom < m_HitBox[i].bottom + m_HitBox[i].height && m_pPlayer->m_Velocity.y < 0.0f && m_pPlayer->m_PlayerShape.bottom > m_HitBox[i].bottom + m_HitBox[i].height / 2)
			{
				m_pPlayer->m_PlayerProps.isOnGround = true;
				m_pPlayer->m_PlayerProps.canJump = true; // This is here because when it gets wrapped to the calculated position there is a problem that you can't jump
				m_pPlayer->m_Velocity.y = 0.0f;
				m_pPlayer->m_PlayerPos.y = m_HitBox[i].bottom + m_HitBox[i].height - m_pPlayer->m_HitBoxOffset.y;
			}
			return true;
		}
	}
	return false;
}

void MovingPlatform::Update(float& elapsedSec, const float& fpsTarget)
{
	GameObject::Update(elapsedSec, fpsTarget);

	for (size_t i{ 0 }; i < m_MinMaxPositions.size(); ++i)
	{
		if (m_MinMaxPositions[i].first.y == m_MinMaxPositions[i].second.y)
		{
			// Cast to int because float comparison is bug prone
			if (m_pSprites[i]->m_SpritePosition.x <= m_MinMaxPositions[i].first.x || m_pSprites[i]->m_SpritePosition.x >= m_MinMaxPositions[i].second.x) m_Speeds[i] = -m_Speeds[i];
			m_pSprites[i]->m_SpritePosition.x += m_Speeds[i] * m_DeltaTime;
			m_HitBox[i].left = m_pSprites[i]->m_SpritePosition.x;
			if (utils::IsOverlapping(m_HitBox[i], m_pPlayer->m_HitBox)) m_pPlayer->m_PlayerPos.x += m_Speeds[i] * m_DeltaTime;
		}
		if (m_MinMaxPositions[i].first.x == m_MinMaxPositions[i].second.x)
		{
			// Cast to int because float comparison is bug prone
			if (m_pSprites[i]->m_SpritePosition.y <= m_MinMaxPositions[i].first.y || m_pSprites[i]->m_SpritePosition.y >= m_MinMaxPositions[i].second.y) m_Speeds[i] = -m_Speeds[i];
			m_pSprites[i]->m_SpritePosition.y += m_Speeds[i] * m_DeltaTime;
			m_HitBox[i].bottom = m_pSprites[i]->m_SpritePosition.y;
			if (utils::IsOverlapping(m_HitBox[i], m_pPlayer->m_HitBox)) m_pPlayer->m_PlayerPos.y += m_Speeds[i] * m_DeltaTime;
		}
	}
}

void MovingPlatform::Initialize(const std::string& minMaxPosPath)
{
	std::ifstream file(minMaxPosPath.c_str());

	for (std::string line; std::getline(file, line); ) //read stream line by line
	{
		std::istringstream in(line);
		Point2f a;
		Point2f b;
		in >> a.x >> a.y >> b.x >> b.y;
		m_MinMaxPositions.push_back(std::make_pair(a, b));
	}

	int sign{ 1 }; // Used to invert the sign of every other speed variable

	for (size_t i{ 0 }; i < m_MinMaxPositions.size(); ++i)
	{
		m_Speeds.push_back(m_Speed * sign);
		sign = -sign;
	}
}
