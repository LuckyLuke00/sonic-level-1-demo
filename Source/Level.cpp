#include "pch.h"
#include "Level.h"
#include "SVGParser.h"

Level::Level(const Window& window, const float& fpsTarget)
	: m_LevelTime{ 0.0f }
	, m_FpsTarget{ fpsTarget }
	, m_Scale{ window.height / 240.0f }
	, m_WindowHeight{ window.height }
	, m_WindowWidth{ window.width }
	, m_StartPos{ 84.0f, 318.0f }
	, m_GameObjectPath{ "../Resources/Sprites/Game Objects/Location Data/" }
	, m_TexturesPath{ "../Resources/Sprites/Level/" }
	, m_pBackgroundTexture{ new Texture{ m_TexturesPath + "GHZ_Sky_Texture.png" } }
	, m_pBottomOceanTexture{ new Texture{ m_TexturesPath + "GHZ_Ocean_Bottom_Texture.png" } }
	, m_pLevelTexture{ new Texture{ m_TexturesPath + "GHZ_Level_Texture.png" } }
	, m_pTopOceanTexture{ new Texture{ m_TexturesPath + "GHZ_Ocean_Top_Texture.png" } }
	, m_pWaterfallTexture{ new Texture{ m_TexturesPath + "GHZ_Waterfall_Texture.png" } }
	, m_DeltaTime{ 0.0f }
	, m_PlayerAngle{}
	, m_FallingPlatform{ m_GameObjectPath + "GHZ_Falling_Platform_Locations.txt", &m_Player }
	, m_Mayflower{ m_GameObjectPath + "GHZ_Mayflower_Locations.txt" }
	, m_MovingPlatform{ m_GameObjectPath + "GHZ_Moving_Platform_Locations.txt", m_GameObjectPath + "GHZ_Moving_Platform_MinMax_Locations.txt", &m_Player }
	, m_Player{ m_StartPos, fpsTarget }
	, m_Rings{ m_GameObjectPath + "GHZ_Ring_Locations.txt", &m_Player }
	, m_Spikes{ m_GameObjectPath + "GHZ_Big_Spikes_Locations.txt",m_GameObjectPath + "GHZ_Small_Spikes_Locations.txt", &m_Player }
	, m_Springs{ m_GameObjectPath + "GHZ_Springs_Yellow_Locations.txt", &m_Player }
	, m_Level{ m_pLevelTexture }
	, m_Sunflowers{ m_GameObjectPath + "GHZ_Sunflower_Locations.txt" }
	, m_Waterfalls{ m_GameObjectPath + "GHZ_Waterfalls_Locations.txt" }
	, m_ItemBoxes{ m_GameObjectPath + "GHZ_Item_Box_Locations.txt", &m_Player, &m_Rings }
	, m_CheckpointLocations{ Point2f{4564.0f, 704.0f}, Point2f{6508.0f, 377.0f} }
	, m_Checkpoints{ m_CheckpointLocations, &m_Player }
	, m_Camera{ window, m_Scale, m_Player.m_PlayerCenter,std::vector<Point2f>{Point2f{0.0f, 276.0f}, Point2f{5888.0f, 0.0f}, Point2f{m_pLevelTexture->GetWidth(), 0.0f}} }
	, m_pLevelSong{ new SoundStream{"../Resources/Soundtrack/Green_Hill_Zone_Original.mp3"} }
	, m_Motobugs{ "../Resources/Sprites/Enemies/Location Data/GHZ_Motobug_Locations.txt", "../Resources/Sprites/Enemies/Location Data/GHZ_Motobug_MinMax_Locations.txt", &m_LevelVertices, &m_Player, &m_Camera }
	, m_Chopper{ "../Resources/Sprites/Enemies/Location Data/GHZ_Chopper_Locations.txt", &m_LevelVertices, &m_Player, &m_Camera }

{
	SVGParser::GetVerticesFromSvgFile("../Resources/Sprites/Level/GHZ_Level_Collision.svg", m_LevelVertices);
	InitBackgroundSprites();
	InitItemBoxes();

	SoundStream::SetVolume(Player::m_Volume);
	m_pLevelSong->Play(true);
}

Level::~Level()
{
	delete m_pBackgroundTexture;
	delete m_pBottomOceanTexture;
	delete m_pLevelTexture;
	delete m_pTopOceanTexture;
	delete m_pWaterfallTexture;
	delete m_pLevelSong;

	m_pBackgroundTexture = nullptr;
	m_pBottomOceanTexture = nullptr;
	m_pLevelTexture = nullptr;
	m_pTopOceanTexture = nullptr;
	m_pWaterfallTexture = nullptr;
	m_pLevelSong = nullptr;

	for (size_t i{ 0 }; i < m_BackgroundSprites.size(); ++i)
	{
		delete m_BackgroundSprites[i];
		m_BackgroundSprites[i] = nullptr;
	}
}

void Level::Draw() const
{
	DrawParallax();

	m_Camera.Transform();
	m_Level.Draw();

	DrawGameObjects();
	m_Player.Draw();

	// Debug Drawing
	if (Player::m_Debug)
	{
		DrawSensors();
		utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
		utils::DrawPoint(m_Player.m_PlayerCenter, 4);
		m_Camera.DrawDebug();
		utils::SetColor(Color4f{ 1.0f, 0.0f, 1.0f, 1.0f });
		for (const auto& i : m_LevelVertices) utils::DrawPolygon(i); // Level collision boxes drawing
	}
	// Debug Drawing
}

void Level::Update(float& elapsedSec)
{
	if (Player::m_Mute)
	{
		SoundStream::SetVolume(0);
	}
	else SoundStream::SetVolume(Player::m_Volume);

	// Set delta time
	m_DeltaTime = elapsedSec * m_FpsTarget;

	// Handle sound pausing
	if (m_Player.m_PlayerProps.invincibilityShield || m_Player.m_PlayerProps.startLevelUpSoundTimer) SoundStream::Stop();
	else if (m_Player.m_PlayerProps.InvincibilityTimer < 0.0f || m_Player.m_PlayerProps.levelUpSoundTimer < 0.0f)
	{
		m_Player.m_PlayerProps.InvincibilityTimer = 0.0f;
		m_Player.m_PlayerProps.levelUpSoundTimer = 0.0f;
		m_pLevelSong->Play(true);
	}

	glPushMatrix();
	glTranslatef(m_Player.m_PlayerCenter.x, m_Player.m_PlayerCenter.y, 0.0f);
	glRotatef(-m_PlayerAngle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-m_Player.m_PlayerCenter.x, -m_Player.m_PlayerCenter.y, 0.0f);

	// Checks if the player has died and performs the appropriate reset
	CheckDeath();

	// Player related updates
	m_Player.Update(elapsedSec);
	UpdateSensors();

	// If the player hasn't died perform collision checking
	if (!m_Player.m_PlayerProps.hasDied) HandleCollision();

	// Game object related updates
	UpdateGameObjects(elapsedSec);

	glPopMatrix();

	// Camera updates
	m_Camera.Update(m_Player.m_PlayerCenter, m_DeltaTime, m_Player.m_PlayerProps.isOnGround);
	HandleParallax(elapsedSec);

	if (!m_Player.m_PlayerProps.deathAnimation) m_LevelTime += elapsedSec;
}

int Level::GetRings() const
{
	return m_Player.m_PlayerProps.rings;
}

int Level::GetScore() const
{
	return m_Player.m_PlayerProps.score;
}

void Level::HandleCollision()
{
	utils::HitInfo hitInfo{};

	if (IsOnGround(hitInfo))
	{
		m_Player.m_Velocity.y = 0.0f;
		m_Player.m_PlayerPos.y = hitInfo.intersectPoint.y;
		m_Player.m_PlayerProps.isOnGround = true;
		m_Player.m_PlayerProps.Spring = false;
	}
	else m_Player.m_PlayerProps.isOnGround = false;

	if (IsPushing(hitInfo) || m_Springs.HandleCollision())
	{
		// Only if the player keeps trying to move while a collision is detected, the player is pushing
		if (m_Player.m_Velocity.x != 0.0f) m_Player.m_PlayerProps.isPushing = true;
		else m_Player.m_PlayerProps.isPushing = false;

		m_Player.m_Velocity.x = 0.0f;

		// Set the players x-position accordingly
		m_Player.m_PlayerPos.x = hitInfo.intersectPoint.x - (hitInfo.intersectPoint.x - m_Player.m_PlayerPos.x);
	}
	else m_Player.m_PlayerProps.isPushing = false;

	// Level Bounds
	if (m_Player.m_HitBox.left + m_Player.m_HitBox.width > m_pLevelTexture->GetWidth())
	{
		m_Player.m_PlayerPos.x -= (m_Player.m_HitBox.left + m_Player.m_HitBox.width) - m_pLevelTexture->GetWidth();
		m_Player.m_Velocity.x = 0.0f;
	}
	if (m_Player.m_HitBox.left < 0.0f)
	{
		m_Player.m_PlayerPos.x -= m_Player.m_HitBox.left;
		m_Player.m_Velocity.x = 0.0f;
	}

	// Game objects collision
	m_Checkpoints.HandleCollision();
	m_FallingPlatform.HandleCollision();
	m_MovingPlatform.HandleCollision();
	m_Rings.HandleCollision();
	m_Spikes.HandleCollision();

	//CalculateAngle(hitInfo);
}

bool Level::IsOnGround(utils::HitInfo& hitInfo) const
{
	for (const auto& i : m_LevelVertices)
	{
		bool colision{ false };

		if (Raycast(i, m_Sensors.m_RightFloorSensor.from, m_Sensors.m_RightFloorSensor.to, hitInfo) && m_Player.
			m_Velocity.y <= 0.0f)
			colision = true;

		float highest{ hitInfo.intersectPoint.y };

		if (Raycast(i, m_Sensors.m_LeftFloorSensor.from, m_Sensors.m_LeftFloorSensor.to, hitInfo) && m_Player.m_Velocity
			.y <= 0.0f)
			colision = true;

		hitInfo.intersectPoint.y = std::max(highest, hitInfo.intersectPoint.y);

		if (colision) return true;
	}
	return false;
}

bool Level::IsPushing(utils::HitInfo& hitInfo) const
{
	for (const auto& i : m_LevelVertices)
	{
		if (m_Player.m_PlayerProps.movingLeft)
		{
			if (Raycast(i, m_Sensors.m_LeftSensor.from, m_Sensors.m_LeftSensor.to, hitInfo)) return true;
		}
		else if (Raycast(i, m_Sensors.m_RightSensor.from, m_Sensors.m_RightSensor.to, hitInfo)) return true;
	}
	return false;
}

void Level::CheckDeath()
{
	if (m_Player.m_PlayerProps.softReset || m_Player.m_PlayerProps.hardReset)
	{
		m_Rings.Reset();
		m_pLevelSong->Play(true);
		m_ItemBoxes.Reset();
		m_Motobugs.Reset();
		m_Chopper.Reset();

		m_Player.m_PlayerProps.score = 0;

		m_Player.m_PlayerProps.softReset = false;

		if (m_Player.m_PlayerProps.hardReset)
		{
			m_Player.m_RespawnPoint = m_StartPos;
			m_Player.m_PlayerProps.hardReset = false;
			m_Checkpoints.Reset();
		}

		// If the players re-spawn point is equal to the start position set level time back to zero
		// (The y check is currently not needed because all the possible re-spawn points have different x-values but it's here for safety)
		if (m_Player.m_RespawnPoint.x == m_StartPos.x && m_Player.m_RespawnPoint.y == m_StartPos.y) m_LevelTime = 0.0f;
	}
}

int Level::GetLives() const
{
	return m_Player.m_PlayerProps.lives;
}

void Level::UpdateSensors()
{
	m_Sensors.m_LeftFloorSensor = Linef{ Point2f{m_Player.m_HitBox.left, m_Player.m_PlayerCenter.y},Point2f{m_Player.m_HitBox.left, m_Player.m_PlayerPos.y - 1} };
	m_Sensors.m_LeftSensor = Linef{ Point2f{m_Player.m_PlayerCenter.x, m_Player.m_PlayerCenter.y},Point2f{m_Player.m_HitBox.left - 1, m_Player.m_PlayerCenter.y} };
	m_Sensors.m_RightFloorSensor = Linef{ Point2f{m_Player.m_HitBox.left + m_Player.m_HitBox.width, m_Player.m_PlayerCenter.y},Point2f{m_Player.m_HitBox.left + m_Player.m_HitBox.width, m_Player.m_PlayerPos.y - 1} };
	m_Sensors.m_RightSensor = Linef{ Point2f{m_Player.m_PlayerCenter.x, m_Player.m_PlayerCenter.y},Point2f{m_Player.m_HitBox.left + m_Player.m_HitBox.width + 1, m_Player.m_PlayerCenter.y} };
}

void Level::InitItemBoxes()
{
	m_ItemBoxes.AddInvincibilityBoxLocations(m_GameObjectPath + "GHZ_Item_Box_Invincibility_Locations.txt");
	m_ItemBoxes.AddLifeBoxLocations(m_GameObjectPath + "GHZ_Item_Box_Life_Locations.txt");
	m_ItemBoxes.AddRingBoxLocations(m_GameObjectPath + "GHZ_Item_Box_Ring_Locations.txt");
}

void Level::UpdateGameObjects(float& elapsedSec)
{
	m_Checkpoints.Update(elapsedSec, m_FpsTarget);
	m_FallingPlatform.Update(elapsedSec, m_FpsTarget);
	m_Mayflower.Update(elapsedSec, m_FpsTarget);
	m_MovingPlatform.Update(elapsedSec, m_FpsTarget);
	m_Rings.Update(elapsedSec, m_FpsTarget);
	m_Spikes.Update(elapsedSec, m_FpsTarget);
	m_Springs.Update(elapsedSec, m_FpsTarget);
	m_Sunflowers.Update(elapsedSec, m_FpsTarget);
	m_Waterfalls.Update(elapsedSec, m_FpsTarget);
	m_ItemBoxes.Update(elapsedSec, m_FpsTarget);
	m_Motobugs.Update(elapsedSec, m_FpsTarget);
	m_Chopper.Update(elapsedSec, m_FpsTarget);
}

void Level::CalculateAngle(utils::HitInfo& hitInfo)
{
	//const Vector2f sensorVec{
	//	m_Sensors.m_LeftFloorSensor.from.x - m_Sensors.m_LeftFloorSensor.to.x,
	//	m_Sensors.m_LeftFloorSensor.from.y - m_Sensors.m_LeftFloorSensor.to.y
	//};
	//const float delta = (hitInfo.normal.AngleWith(sensorVec) * 180.0f / static_cast<float>(M_PI)) - m_PlayerAngle;
	//m_PlayerAngle += delta;

	//m_Player.m_PlayerProps.angle = m_PlayerAngle;

	//std::cout << m_pPlayer.m_PlayerProps.angle << '\n';
}

void Level::DrawGameObjects() const
{
	m_Checkpoints.Draw();
	m_FallingPlatform.Draw();
	m_Mayflower.Draw();
	m_MovingPlatform.Draw();
	m_Waterfalls.Draw();
	m_Rings.Draw();
	m_Spikes.Draw();
	m_Springs.Draw();
	m_Sunflowers.Draw();
	m_ItemBoxes.Draw();
	m_Motobugs.Draw();
	m_Chopper.Draw();
}

void Level::HandleParallax(float& elapsedSec)
{
	m_BackgroundSprites[0]->m_SpritePosition.x -= 0.9f * m_DeltaTime; // Top clouds
	m_BackgroundSprites[1]->m_SpritePosition.x -= 0.45f * m_DeltaTime; // Middle clouds
	m_BackgroundSprites[2]->m_SpritePosition.x -= 0.25f * m_DeltaTime; // Bottom clouds
	m_BackgroundSprites[3]->m_SpritePosition.x -= m_Camera.m_CameraVelocity.x * 0.001f; // Mountains
	m_BackgroundSprites[4]->m_SpritePosition.x -= m_Camera.m_CameraVelocity.x * 0.0015f; // Waterfall
	m_BackgroundSprites[5]->m_SpritePosition.x -= m_Camera.m_CameraVelocity.x * 0.00175f; // Top ocean
	m_BackgroundSprites[6]->m_SpritePosition.x -= m_Camera.m_CameraVelocity.x * 0.00195f; // Bottom ocean

	for (const auto& i : m_BackgroundSprites)
	{
		i->Wrap(m_WindowWidth);
		i->Update(elapsedSec);
	}
}

void Level::InitBackgroundSprites()
{
	const float yPos{ m_WindowHeight / m_Scale };

	m_BackgroundSprites.push_back(new SpriteManager{ m_pBackgroundTexture, m_pBackgroundTexture->GetWidth(), 32.0f, Point2f{0.0f, yPos - 32.0f}, m_Scale }); // Top clouds
	m_BackgroundSprites.push_back(new SpriteManager{ m_pBackgroundTexture, m_pBackgroundTexture->GetWidth(), 16.0f, Point2f{0.0f, yPos - 48.0f}, m_Scale, 80.0f }); // Middle clouds
	m_BackgroundSprites.push_back(new SpriteManager{ m_pBackgroundTexture, m_pBackgroundTexture->GetWidth(), 16.0f, Point2f{0.0f, yPos - 64.0f}, m_Scale, 64.0f }); // Bottom clouds
	m_BackgroundSprites.push_back(new SpriteManager{ m_pBackgroundTexture, m_pBackgroundTexture->GetWidth(), 48.0f, Point2f{0.0f, yPos - 112.0f}, m_Scale, 48.0f }); // Mountains
	m_BackgroundSprites.push_back(new SpriteManager{ m_pWaterfallTexture, m_pWaterfallTexture->GetWidth(), 42.0f, Point2f{0.0f, yPos - 154.0f}, m_Scale }); // Waterfalls
	m_BackgroundSprites.push_back(new SpriteManager{ m_pTopOceanTexture, m_pTopOceanTexture->GetWidth(), 29.0f, Point2f{0.0f, yPos - 183.0f}, m_Scale }); // Ocean top
	m_BackgroundSprites.push_back(new SpriteManager{ m_pBottomOceanTexture, m_pTopOceanTexture->GetWidth(), 89.0f, Point2f{0.0f, yPos - 272.0f}, m_Scale }); // Ocean bottom

	m_BackgroundSprites[4]->ChangeSprite(1, 4, 1, 1, 10); // Loop waterfall sprite
	m_BackgroundSprites[5]->ChangeSprite(1, 4, 1, 1, 10); // Loop top ocean sprite
	m_BackgroundSprites[6]->ChangeSprite(1, 4, 1, 1, 10); // Loop bottom ocean sprite
}

void Level::DrawParallax() const
{
	for (const auto& i : m_BackgroundSprites) i->Draw();
}

void Level::DrawSensors() const
{
	utils::SetColor(Color4f{ 0.0f, 0.941f, 0.f, 1.0f });
	utils::DrawLine(m_Sensors.m_LeftFloorSensor.from, m_Sensors.m_LeftFloorSensor.to, 2.0f);
	utils::SetColor(Color4f{ 0.22f, 1.0f, 0.635f, 1.0f });
	utils::DrawLine(m_Sensors.m_RightFloorSensor.from, m_Sensors.m_RightFloorSensor.to, 2.0f);
	utils::SetColor(Color4f{ 1.0f, 0.329f, 0.329f, 1.0f });
	utils::DrawLine(m_Sensors.m_RightSensor.from, m_Sensors.m_RightSensor.to, 2.0f);
	utils::SetColor(Color4f{ 1.0f, 0.22f, 1.0f, 1.0f });
	utils::DrawLine(m_Sensors.m_LeftSensor.from, m_Sensors.m_LeftSensor.to, 2.0f);
}
