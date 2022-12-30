#pragma once
#include "NumberManager.h"
#include "SpriteManager.h"

class Texture;

class UI final
{
public:
	explicit UI(const float& windowWidth, const float& windowHeight);

	UI(const UI& other) = delete;
	UI& operator=(const UI& other) = delete;
	UI(UI&& other) = delete;
	UI& operator=(UI&& other) = delete;

	~UI();

	void Draw() const;
	void Update(float& elapsedSec, float& levelTime, const int& rings, const int& lives, const int& score);
private:
	Texture* m_HUDTexture;
	Texture* m_RingFlashTexture;

	const float m_Scale;

	float m_Time;

	int m_TimeMiliSec;
	int m_TimeMin;
	int m_TimeSec;

	NumberManager m_Lives;
	NumberManager m_Rings;
	NumberManager m_Score;

	NumberManager m_MiliSeconds;
	NumberManager m_Minutes;
	NumberManager m_Seconds;

	SpriteManager m_HUD;
	SpriteManager m_RingFlashSprite;

	void UpdateTime(float& levelTime);
};
