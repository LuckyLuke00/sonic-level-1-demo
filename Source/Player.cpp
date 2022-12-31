#include "pch.h"
#include "Player.h"
#include <algorithm>
#include "Constants.h"
#include "Texture.h"

#include "SoundEffect.h"
#include "SoundStream.h"
#include "utils.h"

bool Player::m_Debug{ false };
bool Player::m_Mute{ false };
int Player::m_Volume{ 75 };

Player::Player(const Point2f& startPos, const float& fpsTarget)
	: m_GroundSpeed{ 0.0f }
	, m_PlayerCenter{ 0.0f, 0.0f }
	, m_PlayerPos{ startPos }
	, m_RespawnPoint{ startPos }
	, m_StartPos{ startPos }
	, m_BallHitBox{ 0.0f, 0.0f, 17.0f, 23.0f }
	, m_UprightHitBox{ m_PlayerPos.x, m_PlayerPos.y, 17.0f, 33.0f }
	, m_HitBox{ m_UprightHitBox }
	, m_PlayerShape{ 0.0f, 0.0f, 19.0f, 39.0f }
	, m_HitBoxOffset{ 16.0f, 3.0f }
	, m_Velocity{ 0.0f, 0.0f }
	, m_pPlayerTexture{ new Texture{"../Resources/Sprites/Entities/Sonic.png"} }
	, m_pInvincibilityTexture{ new Texture{ "../Resources/Sprites/Entities/Invincibility_Sparkles.png" } }
	, m_AirFactor{ 1.25f }
	, m_DeltaTime{}
	, m_FpsTarget{ fpsTarget }
	, m_pDeathSound{ new SoundEffect{"../Resources/Sound Effects/Hurt.wav"} }
	, m_pExtraLifeSound{ new SoundEffect{ "../Resources/Soundtrack/1-Up.ogg"} }
	, m_pGameOverSound{ new SoundEffect{ "../Resources/Soundtrack/Game-Over.ogg" } }
	, m_pInvincibilitySound{ new SoundEffect{ "../Resources/Soundtrack/Invincibility.ogg" } }
	, m_pJumpSound{ new SoundEffect{ "../Resources/Sound Effects/Jump.wav" } }
	, m_PlayerSprite{ m_pPlayerTexture, 48.0f, 48.0f, startPos }
	, m_InvincibilitySprite{ m_pInvincibilityTexture, 48.0f, 48.0f }
{
	m_pJumpSound->SetVolume(m_Volume);
	m_InvincibilitySprite.ChangeFps(0);
}

Player::~Player()
{
	delete m_pDeathSound;
	delete m_pExtraLifeSound;
	delete m_pGameOverSound;
	delete m_pInvincibilitySound;
	delete m_pInvincibilityTexture;
	delete m_pJumpSound;
	delete m_pPlayerTexture;

	m_pDeathSound = nullptr;
	m_pExtraLifeSound = nullptr;
	m_pGameOverSound = nullptr;
	m_pInvincibilitySound = nullptr;
	m_pInvincibilityTexture = nullptr;
	m_pJumpSound = nullptr;
	m_pPlayerTexture = nullptr;
}

void Player::Draw() const
{
	m_PlayerSprite.Draw();
	m_InvincibilitySprite.Draw();

	if (m_Debug)
	{
		utils::SetColor(Color4f{ 1.0f,0.0f,0.0f,0.25f });
		utils::FillRect(m_HitBox);
		utils::DrawPoint(m_PlayerCenter, 5);
	}
}

void Player::Update(float& elapsedSec)
{
	if (m_Mute)
	{
		m_pDeathSound->SetVolume(0);
		m_pExtraLifeSound->SetVolume(0);
		m_pGameOverSound->SetVolume(0);
		m_pInvincibilitySound->SetVolume(0);
		m_pJumpSound->SetVolume(0);
	}
	else
	{
		m_pDeathSound->SetVolume(Player::m_Volume);
		m_pExtraLifeSound->SetVolume(Player::m_Volume);
		m_pGameOverSound->SetVolume(Player::m_Volume);
		m_pInvincibilitySound->SetVolume(Player::m_Volume);
		m_pJumpSound->SetVolume(Player::m_Volume);
	}

	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	m_DeltaTime = elapsedSec * m_FpsTarget;

	PlayerChecks(pStates);

	if (!m_PlayerProps.isHurt && !m_PlayerProps.hasDied) HandleInput(pStates); // While the player is hurt there should be no input

	ApplyVelocity();
	m_PlayerSprite.Update(elapsedSec);

	m_InvincibilitySprite.m_SpritePosition = m_PlayerPos;
	m_InvincibilitySprite.Update(elapsedSec);

	HandleSprites();
}

void Player::ApplyVelocity()
{
	m_PlayerPos.x += m_Velocity.x * m_DeltaTime;
	m_PlayerPos.y += m_Velocity.y * m_DeltaTime;

	m_PlayerSprite.m_SpritePosition.x = m_PlayerPos.x;
	m_PlayerSprite.m_SpritePosition.y = m_PlayerPos.y;

	m_UprightHitBox.left = m_BallHitBox.left = m_PlayerPos.x + m_HitBoxOffset.x;
	m_UprightHitBox.bottom = m_BallHitBox.bottom = m_PlayerPos.y + m_HitBoxOffset.y;

	m_PlayerCenter.x = m_UprightHitBox.left + m_UprightHitBox.width / 2;
	m_PlayerCenter.y = m_UprightHitBox.bottom + m_UprightHitBox.height / 2;

	m_PlayerShape.left = m_PlayerCenter.x - m_PlayerShape.width / 2;
	m_PlayerShape.bottom = m_PlayerCenter.y - m_PlayerShape.height / 2;

	if (!m_PlayerProps.isOnGround) m_Velocity.y -= grv * m_DeltaTime;
}

void Player::HandleInput(const Uint8* pStates)
{
	if (pStates[SDL_SCANCODE_LEFT])
	{
		if (m_Velocity.x > 0) // if moving to the right
		{
			m_Velocity.x -= dec * m_AirFactor * m_DeltaTime; // decelerate
		}
		else if (m_Velocity.x > -top.x) // if moving to the left
		{
			m_PlayerProps.movingLeft = true;
			m_Velocity.x -= acc * m_AirFactor * m_DeltaTime; // accelerate
			if (m_Velocity.x < -top.x) // impose top speed limit
				m_Velocity.x = -top.x;
		}
	}

	if (pStates[SDL_SCANCODE_RIGHT])
	{
		if (m_Velocity.x < 0) // if moving to the left
		{
			m_Velocity.x += dec * m_AirFactor * m_DeltaTime; // decelerate
		}
		else if (m_Velocity.x < top.x) // if moving to the right
		{
			m_PlayerProps.movingLeft = false;
			m_Velocity.x += acc * m_AirFactor * m_DeltaTime; // accelerate
			if (m_Velocity.x > top.x) // impose top speed limit
				m_Velocity.x = top.x;
		}
	}

	if (pStates[SDL_SCANCODE_SPACE] && m_PlayerProps.canJump) // if the space bar is pressed and the space bar has not been released - jump
	{
		m_Velocity.y += jmp * m_DeltaTime;
		if (!m_PlayerProps.isJumping) m_pJumpSound->Play(0); // If statement for is jumping so the sound is only played once
		m_PlayerProps.isJumping = true;
	}

	if (!pStates[SDL_SCANCODE_LEFT] && !pStates[SDL_SCANCODE_RIGHT]) // Ease out acceleration
	{
		m_Velocity.x -= fminf(abs(m_Velocity.x), frc * m_DeltaTime) * Sign(m_Velocity.x);
	}
}

void Player::PlayerChecks(const Uint8* pStates)
{
	if (m_PlayerProps.isOnGround)
	{
		m_PlayerProps.isJumping = false; // When player is on the ground he is no longer jumping - Also prevents jump animation from continuing while on the ground
		m_AirFactor = 1.0f;
	}
	else m_AirFactor = 2.0f;
	if (!pStates[SDL_SCANCODE_SPACE] && m_Velocity.y > 4.0f && !m_PlayerProps.Spring) m_Velocity.y = 4.0f; // Variable jump height when player releases space bar
	if (m_PlayerProps.isOnGround && !pStates[SDL_SCANCODE_SPACE]) // If player is on the ground and is not holding the space bar - player can jump
	{
		m_PlayerProps.canJump = true;
		m_PlayerProps.isJumping = false;
	}
	if (m_Velocity.y >= top.y / maxJmp || m_Velocity.y < 0.0f) m_PlayerProps.canJump = false; // if the velocity is greater than the max jump velocity or lower than zero - player cannot jump

	if (m_PlayerProps.isRolling) m_HitBox = m_BallHitBox;
	else m_HitBox = m_UprightHitBox;

	if (m_PlayerProps.extraLife)
	{
		++m_PlayerProps.lives;
		m_PlayerProps.extraLife = false;
		m_PlayerProps.startLevelUpSoundTimer = true;
		m_pExtraLifeSound->SetVolume(m_Volume);
		m_pExtraLifeSound->Play(0);
	}
	if (m_PlayerProps.startLevelUpSoundTimer) m_PlayerProps.levelUpSoundTimer += m_DeltaTime / m_FpsTarget;
	if (m_PlayerProps.levelUpSoundTimer >= m_PlayerProps.maxLevelUpTime)
	{
		m_PlayerProps.startLevelUpSoundTimer = false;
		m_PlayerProps.levelUpSoundTimer = -1.0f;
	}
	// If the player has died and the death animation has not been started - Handle death event
	if (m_PlayerProps.hasDied) HandleDeath();

	if (m_PlayerProps.invincibilityShield && !m_PlayerProps.startInvincibleTimer)
	{
		m_PlayerProps.invincible = true;
		m_PlayerProps.startInvincibleTimer = true;
		m_InvincibilitySprite.ChangeSprite(1, 1, 1, 4, 16);
		m_pInvincibilitySound->SetVolume(m_Volume);
		m_pInvincibilitySound->Play(0);
	}
	if (m_PlayerProps.startInvincibleTimer) m_PlayerProps.InvincibilityTimer += m_DeltaTime / m_FpsTarget;
	if (m_PlayerProps.InvincibilityTimer >= m_PlayerProps.maxInvincibilityTime)
	{
		m_PlayerProps.invincible = false;
		m_PlayerProps.startInvincibleTimer = false;
		m_PlayerProps.invincibilityShield = false;
		m_PlayerProps.InvincibilityTimer = -1.0f;
		m_InvincibilitySprite.ChangeSprite(1, 1, 1, 4, 0);
		SoundEffect::StopAll();
	}
	HandlePlayerHurtState();
}

void Player::HandleSprites()
{
	if (m_PlayerProps.isJumping) m_PlayerSprite.ChangeSprite(6, 1, 1, 16, 32);
	else if (m_PlayerProps.isOnGround) // If the player is on ground change sprites according to x-speed
	{
		if (m_PlayerProps.isPushing) m_PlayerSprite.ChangeSprite(9, 1, 1, 10, 10);
		else if (abs(m_Velocity.x) < 3.0f) m_PlayerSprite.ChangeSprite(2, 1, 1, 12, 32);
		else if (abs(m_Velocity.x) < 6.0f) m_PlayerSprite.ChangeSprite(3, 1, 1, 10, 32);
		else if (abs(m_Velocity.x) == 6.0f) m_PlayerSprite.ChangeSprite(4, 1, 1, 8, 32);

		m_PlayerSprite.ChangeFps(std::max(abs(m_Velocity.x * (32 / top.x)), 8.0f)); // Calculate fps based on x-speed

		// If the player is on ground and x-speed is zero change sprite to idle and ignore the calculated fps for faster transition
		if (m_Velocity.x == 0) m_PlayerSprite.ChangeSprite(1, 1, 1, 1, 32);
	}

	// If player hit spring and the y-velocity is greater than zero - Change sprite to spring sprite
	if (m_PlayerProps.Spring && m_Velocity.y > 0.0f) m_PlayerSprite.ChangeSprite(10, 1, 1, 10, 32);
	else if (m_PlayerProps.Spring) m_PlayerSprite.ChangeSprite(11, 1, 1, 12, 8); // Change sprite to air walking animation when the player hit spring and y-velocity is below zero

	if (m_PlayerProps.movingLeft) m_PlayerSprite.Mirror();
}

void Player::HandlePlayerHurtState()
{
	if (m_PlayerProps.isHurt)
	{
		m_PlayerSprite.ChangeSprite(1, 1, 3, 1, 32); // Change sprite to hurt sprite
		m_PlayerProps.invincible = true;

		if (m_PlayerProps.isOnGround)
		{
			m_PlayerProps.startHurtTimer = true;
			m_PlayerProps.isHurt = false;
		}
	}

	if (m_PlayerProps.startHurtTimer)
	{
		m_PlayerSprite.m_Flicker = true;
		m_PlayerProps.playerHurtTimer += m_DeltaTime / m_FpsTarget;
	}
	if (m_PlayerProps.playerHurtTimer >= m_PlayerProps.maxHurtTime)
	{
		m_PlayerSprite.m_Flicker = false;
		m_PlayerProps.playerHurtTimer = 0.0f;
		m_PlayerProps.startHurtTimer = false;
		m_PlayerProps.invincible = false;
	}
}

void Player::HandleDeath()
{
	if (!m_PlayerProps.deathAnimation)
	{
		if (!m_PlayerProps.spikeDeath) m_pDeathSound->Play(0);
		m_PlayerSprite.ChangeSprite(1, 1, 2, 1, 32);

		m_Velocity.x = 0.0f;
		m_Velocity.y = 7.0f; // Launch Player up

		m_PlayerProps.deathAnimation = true; // Begin death animation
		m_PlayerProps.isHurt = false; // Prevent player from changing into the hurt sprite
		m_PlayerProps.isJumping = false; // Prevent player from continuing the jumping sprite
		m_PlayerProps.isOnGround = false; // Set to false so gravity will be added
		m_PlayerProps.movingLeft = false; // Set to false so player is facing right when re-spawned
		m_PlayerProps.rings = 0; // If the player has died the amount of rings should be zero
		m_PlayerProps.spikeDeath = false;
		m_PlayerProps.Spring = false;

		SoundStream::Stop(); // Jazz music stops
	}

	if (m_PlayerPos.y + m_PlayerShape.height < 0.0f) // Check if the player has gone off-screen
	{
		if (!m_PlayerProps.startDeathSoundTimer)
		{
			if (--m_PlayerProps.lives <= 0)
			{
				m_PlayerProps.startDeathSoundTimer = true;
				m_pGameOverSound->Play(0);
			}
			else
			{
				m_PlayerProps.softReset = true;
				m_PlayerPos = m_RespawnPoint;

				m_PlayerProps.deathAnimation = false;
				m_PlayerProps.hasDied = false;
				m_PlayerProps.isOnGround = true;
			}
		}
		else if (m_PlayerProps.deathSoundTimer >= m_PlayerProps.maxDeathSoundTime)
		{
			m_PlayerProps.startDeathSoundTimer = false;
			m_PlayerProps.deathSoundTimer = 0.0f;

			m_RespawnPoint = m_StartPos;
			m_PlayerProps.lives = 3;
			m_PlayerPos = m_RespawnPoint;
			m_PlayerProps.hardReset = true;

			m_PlayerProps.deathAnimation = false;
			m_PlayerProps.hasDied = false;
			m_PlayerProps.isOnGround = true;
		}
		if (m_PlayerProps.startDeathSoundTimer) m_PlayerProps.deathSoundTimer += m_DeltaTime / m_FpsTarget;
	}
}

int Player::Sign(float value)
{
	if (value < 0) return -1;
	if (value > 0) return 1;
	return 0;
}
