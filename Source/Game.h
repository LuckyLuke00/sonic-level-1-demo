#pragma once
#include "Level.h"
#include "UI.h"

class Game final
{
public:
	explicit Game(const Window& window);
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game(Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update(float elapsedSec);
	void Draw() const;

	// Event handling
	void ProcessKeyDownEvent(const SDL_KeyboardEvent& e);
	void ProcessKeyUpEvent(const SDL_KeyboardEvent& e);
	void ProcessMouseMotionEvent(const SDL_MouseMotionEvent& e);
	void ProcessMouseDownEvent(const SDL_MouseButtonEvent& e);
	void ProcessMouseUpEvent(const SDL_MouseButtonEvent& e);

private:
	// DATA MEMBERS
	const Window m_Window;

	const float m_FpsTarget; // Fps the animations and physics should run
	Level m_GHZ; // Green Hill Zone
	UI m_UI;

	// FUNCTIONS
	void Initialize();
	void Cleanup();
	void ClearBackground() const;
	void setScale(float scale) const;

	void DisplayMenu() const;
	void DisplayMenuTitle() const;
	void DisplayInstructions() const;
};
