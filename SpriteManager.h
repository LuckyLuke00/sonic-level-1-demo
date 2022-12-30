#pragma once

#include "Texture.h"

class SpriteManager final
{
public:
	explicit SpriteManager(const Texture* texture);
	explicit SpriteManager(const Texture* texture, const float& spriteWidth, const float& spriteHeight,
		const Point2f& position = Point2f{ 0.0f, 0.0f }, const float& scale = 1.0f,
		const float& spriteTop = 0.0f, const float& fps = 60.0f);

	SpriteManager(const SpriteManager& other) = delete;
	SpriteManager& operator=(const SpriteManager& other) = delete;
	SpriteManager(SpriteManager&& other) = delete;
	SpriteManager& operator=(SpriteManager&& other) = delete;

	~SpriteManager() = default;

	bool m_Flicker;
	Point2f m_SpritePosition; // Where the texture(s) should be drawn

	void ChangeFps(float fps);
	void ChangeSprite(int startingRow, int nrRows, int startingCol, int nrCols, float fps, bool reset = false);
	// Change the starting position of the source rect on the sprite sheet
	void ChangeSpriteAfter(int loops, int startingRow, int nrRows, int startingCol, int nrCols, float fps,
		bool reverse = false); // Change sprite after specified amount of loops
	void Draw() const;
	void Mirror();
	void Reverse(); // Set some properties for animations
	void Wrap(const float& windowWidth);

	void Update(float& elapsedSec);

private:
	struct LoopProps
		// We need this store the data that the source rect should change to after a specified amount of loops have been completed
	{
		bool reverse;
		float fps;
		int loops;
		int nrCols;
		int nrRows;
		int startingCol;
		int startingRow;
	};

	bool m_AnimationChanged;
	bool m_LoopAnimation; // Flag to start counting animation loops
	bool m_Mirror;
	bool m_Reverse;

	const Texture* m_pTexture;
	const Point2f m_SpriteDimensions; // Width and height of one sprite frame

	float m_AccumulatedSec;
	float m_FramesPerSec;
	float m_FrameTime;
	float m_Scale;
	float m_SpriteSheetLeft;
	float m_SpriteSheetTop;

	int m_Cols;
	int m_CurFrame;
	int m_LoopCount;
	int m_Rows;

	LoopProps m_SpriteSheetPos;
	Point2f m_SpriteCenter;
	Rectf m_DestRect;
	Rectf m_SrcRect;

	// Sprite related functions
	void LoopAnimation();
	void UpdateDestRect();
	void UpdateSourceRect();

	// Frame rate
	bool OnUpdate(float& elapsedSec);
};
