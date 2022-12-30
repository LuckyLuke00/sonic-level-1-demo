#include "pch.h"
#include "UI.h"

#include "Texture.h"

UI::UI(const float& windowWidth, const float& windowHeight)
	: m_HUDTexture{ new Texture{"../Resources/UI/UI_Overlay.png"} }
	, m_RingFlashTexture{ new Texture{"../Resources/UI/UI_Rings_Overlay.png"} }
	, m_Scale{ windowHeight / m_HUDTexture->GetHeight() }
	, m_Time{ 0 }
	, m_TimeMiliSec{ 0 }
	, m_TimeMin{ 0 }
	, m_TimeSec{ 0 }
	, m_Lives{ Point2f{ 52.0f, 12.0f }, m_Scale, 3 }
	, m_Rings{ Point2f{ 109.0f, 182.0f }, m_Scale }
	, m_Score{ Point2f{109.0f, 214.0f }, m_Scale }
	, m_MiliSeconds{ Point2f{ 109.0f, 198.0f }, m_Scale, 0, 2 }
	, m_Minutes{ Point2f{ 59.0f, 198.0f}, m_Scale, 0 }
	, m_Seconds{ Point2f{ 84.0f, 198.0f}, m_Scale, 0, 2 }
	, m_HUD{ m_HUDTexture, m_HUDTexture->GetWidth(), m_HUDTexture->GetHeight(), Point2f{}, m_Scale }
	, m_RingFlashSprite{ m_RingFlashTexture, m_RingFlashTexture->GetWidth() / 2, m_RingFlashTexture->GetHeight(), Point2f{16.0f, 182.0f}, m_Scale }
{
}

UI::~UI()
{
	delete m_HUDTexture;
	delete m_RingFlashTexture;

	m_HUDTexture = nullptr;
	m_RingFlashTexture = nullptr;
}

void UI::Draw() const
{
	m_HUD.Draw();
	m_RingFlashSprite.Draw();
	m_Rings.Draw();
	m_Lives.Draw();
	m_Score.Draw();

	m_MiliSeconds.Draw();
	m_Seconds.Draw();
	m_Minutes.Draw();
}

void UI::Update(float& elapsedSec, float& levelTime, const int& rings, const int& lives, const int& score)
{
	UpdateTime(levelTime);

	if (rings == 0) m_RingFlashSprite.ChangeSprite(1, 1, 1, 2, 8);
	else m_RingFlashSprite.ChangeSprite(1, 1, 1, 1, 8);

	m_HUD.Update(elapsedSec);
	m_RingFlashSprite.Update(elapsedSec);
	m_Rings.Update(rings, elapsedSec);
	m_Lives.Update(lives, elapsedSec);
	m_Score.Update(score, elapsedSec);

	m_MiliSeconds.Update(m_TimeMiliSec, elapsedSec);
	m_Seconds.Update(m_TimeSec, elapsedSec);
	m_Minutes.Update(m_TimeMin, elapsedSec);

	// Check if the displayed numbers should be lowered for example:
	// if ring amount was 11 and it gets reset back to 0
	// it would display "00" instead of "0" - This function prevents that
	m_Rings.Reset();
	m_Score.Reset();
}

void UI::UpdateTime(float& levelTime)
{
	m_Time = levelTime;

	m_TimeMin = static_cast<int>(m_Time / 60);
	m_TimeSec = static_cast<int>(m_Time);
	m_TimeMiliSec = static_cast<int>((m_Time - m_TimeSec) * 100);

	m_TimeSec %= 60;
}
