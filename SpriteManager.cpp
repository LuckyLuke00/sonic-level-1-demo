#include "pch.h"
#include "SpriteManager.h"
#include "Texture.h"

SpriteManager::SpriteManager(const Texture* texture)
	: SpriteManager{ texture, texture->GetWidth(), texture->GetHeight() }
{
}

//SpriteManager::SpriteManager(const Texture* texture, const float& spriteWidth, const float& spriteHeight, const float& fps, const float& scale)
//	: SpriteManager{ texture, spriteWidth, spriteHeight, fps,  Point2f{ 0.0f, 0.0f }, scale }
//{
//}

SpriteManager::SpriteManager(const Texture* texture, const float& spriteWidth, const float& spriteHeight, const Point2f& position, const float& scale, const float& spriteTop, const float& fps)
	: m_Flicker{ false }
	, m_SpritePosition{ position }
	, m_AnimationChanged{ false }
	, m_LoopAnimation{ false }
	, m_Mirror{ false }
	, m_Reverse{ false }
	, m_pTexture{ texture }
	, m_SpriteDimensions{ spriteWidth, spriteHeight }
	, m_AccumulatedSec{ 0.0f }
	, m_FramesPerSec{ fps }
	, m_FrameTime{ 1.0f }
	, m_Scale{ scale }
	, m_SpriteSheetLeft{ 0.0f }
	, m_SpriteSheetTop{ texture->GetHeight() - spriteTop }
	, m_Cols{ 1 }
	, m_CurFrame{ 0 }
	, m_LoopCount{ 0 }
	, m_Rows{ 1 }
	, m_SpriteSheetPos{}
	, m_SpriteCenter{ 0.0f , 0.0f }
	, m_DestRect{ position.x, position.y, spriteWidth, spriteHeight }
	, m_SrcRect{ 0.0f, 0.0f, spriteWidth, spriteHeight }
{
	UpdateSourceRect();
}

void SpriteManager::Update(float& elapsedSec)
{
	if (OnUpdate(elapsedSec))
	{
		if (m_Reverse)
		{
			if (--m_CurFrame < 0) m_CurFrame = m_Cols * m_Rows - 1;
		}
		else
		{
			++m_CurFrame %= m_Cols * m_Rows;
		}

		if (m_LoopAnimation) LoopAnimation();

		if (m_AnimationChanged) // if animation has changed set current frame to zero otherwise it will skip a frame
		{
			if (m_Reverse) m_CurFrame = m_Cols * m_Rows - 1;
			else m_CurFrame = 0;

			m_AnimationChanged = false;
		}
		UpdateSourceRect();
	}
	UpdateDestRect();

	m_SpriteCenter.x = m_SpritePosition.x + m_SpriteDimensions.x / 2;
	m_SpriteCenter.y = m_SpritePosition.y + m_SpriteDimensions.y / 2;

	m_Mirror = false;
}

void SpriteManager::Draw() const
{
	glPushMatrix();
	glScalef(m_Scale, m_Scale, 1.0f);
	if (m_Mirror)
	{
		glTranslatef(m_SpriteCenter.x, 0.0f, 0.0f);
		glScalef(-1.0f, 1.0f, 1.0f);
		glTranslatef(-m_SpriteCenter.x, 0.0f, 0.0f);
	}
	if (m_Flicker)
	{
		if (m_CurFrame % 2 == 0) m_pTexture->Draw(m_DestRect, m_SrcRect);
	}
	else if (m_FramesPerSec > 0.0f) m_pTexture->Draw(m_DestRect, m_SrcRect);
	glPopMatrix();
}

void SpriteManager::Reverse()
{
	m_Reverse = !m_Reverse;
}

void SpriteManager::Wrap(const float& windowWidth)
{
	if (m_SpritePosition.x > 0.0f) m_SpritePosition.x = -(m_pTexture->GetWidth() / 2);
	if (m_SpritePosition.x < -m_pTexture->GetWidth() + windowWidth) m_SpritePosition.x = 0.0f;
}

void SpriteManager::Mirror()
{
	m_Mirror = true;
}

void SpriteManager::UpdateSourceRect()
{
	m_SrcRect.bottom = m_SpriteSheetTop + (m_CurFrame / m_Cols + 1) * m_SrcRect.height;
	m_SrcRect.left = m_SpriteSheetLeft + m_CurFrame % m_Cols * m_SrcRect.width;
}

bool SpriteManager::OnUpdate(float& elapsedSec)
{
	m_AccumulatedSec += elapsedSec;

	if (m_FramesPerSec > 0.0f)
	{
		m_FrameTime = 1.0f / m_FramesPerSec;

		if (m_AccumulatedSec > m_FrameTime)
		{
			m_AccumulatedSec -= m_FrameTime;
			return true;
		}
		return false;
	}
	m_FrameTime = 0.0f; // When the fps is equal to zero no sprite is drawn
	return false;
}

void SpriteManager::UpdateDestRect()
{
	m_DestRect.bottom = m_SpritePosition.y;
	m_DestRect.left = m_SpritePosition.x;
}

void SpriteManager::LoopAnimation()
{
	if (m_CurFrame == 0 && m_LoopCount > 0)
	{
		--m_LoopCount;
	}
	if (m_LoopCount == 0)
	{
		m_LoopAnimation = false;
		ChangeSprite(m_SpriteSheetPos.startingRow, m_SpriteSheetPos.nrRows, m_SpriteSheetPos.startingCol, m_SpriteSheetPos.nrCols, m_SpriteSheetPos.fps);
		if (m_SpriteSheetPos.reverse) Reverse();
		m_AnimationChanged = true;
	}
}

void SpriteManager::ChangeSprite(int startingRow, int nrRows, int startingCol, int nrCols, float fps, bool reset)
{
	m_Cols = nrCols;
	m_Rows = nrRows;

	m_SpriteSheetLeft = m_SpriteDimensions.x * (startingCol - 1);
	m_SpriteSheetTop = m_SpriteDimensions.y * (startingRow - 1);

	if (reset) m_CurFrame = 0;

	ChangeFps(fps);

	m_Reverse = false;
}

void SpriteManager::ChangeSpriteAfter(int loops, int startingRow, int nrRows, int startingCol, int nrCols, float fps, bool reverse)
{
	// Set the loop counter and the loop flag
	m_LoopAnimation = true;
	m_LoopCount = loops;

	// Set values in the struct object
	m_SpriteSheetPos.fps = fps;
	m_SpriteSheetPos.loops = loops;
	m_SpriteSheetPos.nrCols = nrCols;
	m_SpriteSheetPos.nrRows = nrRows;
	m_SpriteSheetPos.reverse = reverse;
	m_SpriteSheetPos.startingCol = startingCol;
	m_SpriteSheetPos.startingRow = startingRow;
}

void SpriteManager::ChangeFps(float fps)
{
	m_FramesPerSec = fps;
}
