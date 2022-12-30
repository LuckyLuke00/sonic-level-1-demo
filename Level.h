#pragma once
#include "Camera.h"
#include "Checkpoint.h"
#include "FallingPlatform.h"
#include "ItemBox.h"
#include "Mayflower.h"
#include "MovingPlatform.h"
#include "Player.h"
#include "Rings.h"
#include "SoundStream.h"
#include "Spikes.h"
#include "Springs.h"
#include "SpriteManager.h"
#include "Sunflower.h"
#include "Waterfall.h"
#include "utils.h"
#include "Motobug.h"
#include "Chopper.h"

class Structs;

class Level final
{
public:
	explicit Level(const Window& window, const float& fpsTarget);

	Level(const Level& other) = delete;
	Level& operator=(const Level& other) = delete;
	Level(Level&& other) = delete;
	Level& operator=(Level&& other) = delete;

	~Level();

	void Draw() const;
	void Update(float& elapsedSec);

	int GetLives() const;
	int GetRings() const;
	int GetScore() const;

	void HandleCollision();

	float m_LevelTime;
private:
	struct Sensors
	{
		Linef m_LeftFloorSensor;
		Linef m_LeftSensor;
		Linef m_RightFloorSensor;
		Linef m_RightSensor;
	};

	const float m_FpsTarget;
	const float m_Scale;
	const float m_WindowHeight;
	const float m_WindowWidth;

	const Point2f m_StartPos;

	const std::string m_GameObjectPath;
	const std::string m_TexturesPath;

	const Texture* m_pBackgroundTexture;
	const Texture* m_pBottomOceanTexture;
	const Texture* m_pLevelTexture;
	const Texture* m_pTopOceanTexture;
	const Texture* m_pWaterfallTexture;

	float m_DeltaTime;
	float m_PlayerAngle;

	// Game Objects
	FallingPlatform m_FallingPlatform;
	Mayflower m_Mayflower;
	MovingPlatform m_MovingPlatform;
	Player m_Player;
	Rings m_Rings;
	Sensors m_Sensors;
	Spikes m_Spikes;
	Springs m_Springs;
	SpriteManager m_Level;
	Sunflower m_Sunflowers;
	Waterfall m_Waterfalls;
	ItemBox m_ItemBoxes;

	Motobug m_Motobugs;
	Chopper m_Chopper;
	// Game Objects

	std::vector<Point2f> m_CheckpointLocations;
	std::vector<Point2f> m_Vertices;
	std::vector<SpriteManager*> m_BackgroundSprites;
	std::vector<std::vector<Point2f>> m_LevelVertices;

	Checkpoint m_Checkpoints;

	Camera m_Camera;
	SoundStream* m_pLevelSong{};

	bool IsOnGround(utils::HitInfo& hitInfo) const;
	bool IsPushing(utils::HitInfo& hitInfo) const;
	void CalculateAngle(utils::HitInfo& hitInfo);
	void CheckDeath(); // Death related checks and resets
	void DrawGameObjects() const;
	void DrawParallax() const;
	void DrawSensors() const;
	void HandleParallax(float& elapsedSec);
	void InitBackgroundSprites(); // Creates SpriteManager objects for the background textures
	void UpdateGameObjects(float& elapsedSec);
	void UpdateSensors();
	void InitItemBoxes();
};
