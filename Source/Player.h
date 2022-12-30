#pragma once
#include "SpriteManager.h"

class SoundEffect;

class Player final
{
public:
	struct PlayerProps
	{
		bool canJump{ true };
		bool deathAnimation{ false };
		bool extraLife{ false };
		bool hardReset{ false };
		bool hasDied{ false };
		bool invincibilityShield{ false };
		bool invincible{ false };
		bool isHurt{ false };
		bool isJumping{ false };
		bool isOnGround{ true };
		bool isPushing{ false };
		bool isRolling{ false };
		bool movingLeft{ false };
		bool skid{ false };
		bool softReset{ false };
		bool spikeDeath{ false };
		bool Spring{ false };
		bool startDeathSoundTimer{ false };
		bool startHurtTimer{ false };
		bool startInvincibleTimer{ false };
		bool startLevelUpSoundTimer{ false };
		float angle{ 0.0f };
		float deathSoundTimer{ 0.0f };
		float InvincibilityTimer{ 0.0f };
		float levelUpSoundTimer{ 0.0f };
		float maxDeathSoundTime{ 13.0f };
		float maxHurtTime{ 2.0f };
		float maxInvincibilityTime{ 22.0f };
		float maxLevelUpTime{ 4.0f };
		float playerHurtTimer{ 0.0f };
		int lives{ 3 };
		int rings{ 0 };
		int score{ 0 };
	};

	static bool m_Debug;
	static bool m_Mute;
	static int m_Volume;

	explicit Player(const Point2f& startPos, const float& fpsTarget);

	Player(const Player& other) = delete;
	Player& operator=(const Player& other) = delete;
	Player(Player&& other) = delete;
	Player& operator=(Player&& other) = delete;

	~Player();

	void Draw() const;
	void Update(float& elapsedSec);

	float m_GroundSpeed;

	PlayerProps m_PlayerProps;
	Point2f m_PlayerCenter;
	Point2f m_PlayerPos;
	Point2f m_RespawnPoint; // The point the player should spawn after he died
	Point2f m_StartPos;

	Rectf m_BallHitBox; // Hit box when rolling or jumping
	Rectf m_UprightHitBox; // Hit box when upright
	Rectf m_HitBox; // The hit box that is currently active

	Rectf m_PlayerShape;
	Vector2f m_HitBoxOffset;
	Vector2f m_Velocity;

	static int Sign(float value);

private:
	const Texture* m_pPlayerTexture;
	const Texture* m_pInvincibilityTexture;

	float m_AirFactor; // The factor the acceleration and deceleration should be multiplied by if the player is in the air
	float m_DeltaTime;
	float m_FpsTarget;

	SoundEffect* m_pDeathSound;
	SoundEffect* m_pExtraLifeSound;
	SoundEffect* m_pGameOverSound;
	SoundEffect* m_pInvincibilitySound;
	SoundEffect* m_pJumpSound;

	SpriteManager m_InvincibilitySprite;
	SpriteManager m_PlayerSprite;

	void ApplyVelocity();
	void HandleInput(const Uint8* pStates);
	void PlayerChecks(const Uint8* pStates);
	void HandleSprites();
	void HandlePlayerHurtState();
	void HandleDeath();
};
