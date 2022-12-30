#include "pch.h"
#include "NumberManager.h"

#include "SpriteManager.h"
#include "Texture.h"

NumberManager::NumberManager(const Point2f& numberStartPos, const float& scale, const int& currentNumber, const int& numbersToDraw, bool reversePos)
	: m_ReversePos{ reversePos }
	, m_Scale{ scale }
	, m_SpriteHeight{ 14.0f }
	, m_SpriteWidth{ 9.0f }
	, m_CurrentNumber{ currentNumber }
	, m_NumberSize{ SizeOfInt(currentNumber) }
	, m_NumbersToDraw{ numbersToDraw }
	, m_NumberStartPos{ numberStartPos }
	, m_pNumbersTexture{ new Texture{ "./Resources/UI/UI_Numbers.png" } }
	, m_NumberSprites{ new SpriteManager{ m_pNumbersTexture, m_SpriteWidth, m_SpriteHeight, numberStartPos, m_Scale } }
{
	if (m_NumbersToDraw + 1 != m_NumberSize)
	{
		for (auto i{ m_NumberSize }; i < m_NumbersToDraw; ++i)
		{
			const float newPosX{ m_NumberStartPos.x - m_SpriteWidth * m_NumberSprites.size() };
			m_NumberSprites.push_back(new SpriteManager{ m_pNumbersTexture, m_SpriteWidth, m_SpriteHeight, Point2f{newPosX, m_NumberStartPos.y}, m_Scale });
		}
		m_NumberSize = m_NumbersToDraw;
	}
}

NumberManager::~NumberManager()
{
	for (size_t i{ 0 }; i < m_NumberSprites.size(); ++i)
	{
		delete m_NumberSprites[i];
		m_NumberSprites[i] = nullptr;
	}

	delete m_pNumbersTexture;
	m_pNumbersTexture = nullptr;
}

void NumberManager::Update(const int& number, float& elapsedSec)
{
	if (m_NumberSize < SizeOfInt(number))
	{
		m_CurrentNumber = number;

		AddNumber();

		m_NumberSize = SizeOfInt(number);
	}

	for (const auto& i : m_NumberSprites)
	{
		i->ChangeSprite(1, 1, m_CurrentNumber % 10 + 1, 1, 60);
		m_CurrentNumber /= 10; // Get individual digits
		i->Update(elapsedSec);
	}

	m_CurrentNumber = number; // For safety reset current number
}

void NumberManager::Draw() const
{
	for (const auto& i : m_NumberSprites) i->Draw();
}

int NumberManager::SizeOfInt(int number)
{
	int digits{ 0 };

	do
	{
		++digits;
		number /= 10;
	} while (number);

	return digits;
}

void NumberManager::AddNumber()
{
	for (auto i{ 0 }; i < SizeOfInt(m_CurrentNumber) - m_NumberSize; ++i)
	{
		const float newPosX{ m_NumberStartPos.x - m_SpriteWidth * m_NumberSprites.size() };
		m_NumberSprites.push_back(new SpriteManager{ m_pNumbersTexture, m_SpriteWidth, m_SpriteHeight, Point2f{newPosX, m_NumberStartPos.y}, m_Scale });
	}
}

void NumberManager::Reset()
{
	if (m_CurrentNumber == 0 && static_cast<int>(m_NumberSprites.size()) > m_NumbersToDraw)
	{
		for (auto i{ m_NumberSprites.size() }; static_cast<int>(i) > m_NumbersToDraw; --i)
		{
			delete m_NumberSprites[i - 1];
			m_NumberSprites[i - 1] = nullptr;

			m_NumberSprites.pop_back();
		}
		m_NumberSize = static_cast<int>(m_NumberSprites.size());
	}
}
