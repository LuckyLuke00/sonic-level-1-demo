#include "pch.h"
#include "Game.h"

Game::Game(const Window& window)
	: m_Window{ window }
	, m_FpsTarget{ 60.0f }
	, m_GHZ{ window, m_FpsTarget }
	, m_UI{ window.width, window.height }
{
	Initialize();
}

Game::~Game()
{
	Cleanup();
}

void Game::Initialize()
{
	//DisplayMenuTitle();
	//DisplayMenu();
}

void Game::Cleanup()
{
}

void Game::Update(float elapsedSec)
{
	m_GHZ.Update(elapsedSec);
	m_UI.Update(elapsedSec, m_GHZ.m_LevelTime, m_GHZ.GetRings(), m_GHZ.GetLives(), m_GHZ.GetScore());
}

void Game::Draw() const
{
	ClearBackground();
	glPushMatrix();
	m_GHZ.Draw();
	glPopMatrix();
	m_UI.Draw();
}

void Game::ProcessKeyDownEvent(const SDL_KeyboardEvent& e)
{
}

void Game::ProcessKeyUpEvent(const SDL_KeyboardEvent& e)
{
	switch (e.keysym.sym)
	{
	case SDLK_i:
		DisplayInstructions();
		break;
	case SDLK_BACKSPACE:
		DisplayMenuTitle();
		DisplayMenu();
		break;
	case SDLK_m:
		Player::m_Mute = !Player::m_Mute;
		break;
	case SDLK_UP:
		if (Player::m_Volume > 0) Player::m_Volume += 5;
		break;
	case SDLK_DOWN:
		if (Player::m_Volume > 0) Player::m_Volume -= 5;
		break;
	case SDLK_x:
		Player::m_Debug = !Player::m_Debug;
		break;
	}
}

void Game::ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e)
{
}

void Game::ProcessMouseDownEvent(const SDL_MouseButtonEvent& e)
{
}

void Game::ProcessMouseUpEvent(const SDL_MouseButtonEvent& e)
{
}

void Game::ClearBackground() const
{
	glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Game::setScale(float scale) const
{
	glScalef(scale, scale, 1);
}

void Game::DisplayMenu() const
{
	std::cout << "[I] Instructions\n";
	std::cout << "[X] Debug Overlay\n\n";
	std::cout << "[M] Mute Sound\n";
	std::cout << "[UP] Increase Volume\n";
	std::cout << "[DOWN] Decrease Volume\n";
}

void Game::DisplayMenuTitle() const
{
	system("cls");
	std::cout << "==========================\n";
	std::cout << "    Sonic The Hedgehog\n";
	std::cout << "   Lucas Kinoo - 1DAE21\n";
	std::cout << "==========================\n";
}

void Game::DisplayInstructions() const
{
	system("cls");
	DisplayMenuTitle();
	std::cout << "[->] Move Right\n";
	std::cout << "[<-] Move Left\n";
	std::cout << "[SPACE] Jump\n\n";
	std::cout << "[BACKSPACE] Back\n";
}
