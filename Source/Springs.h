#pragma once
#include "GameObject.h"
class SoundEffect;
class Player;

class Springs final :
	public GameObject
{
public:
	explicit Springs(const std::string& SpringLocationsPath, Player* player);

	Springs(const Springs& other) = delete;
	Springs& operator=(const Springs& other) = delete;
	Springs(Springs&& other) = delete;
	Springs& operator=(Springs&& other) = delete;

	~Springs() override;

	bool HandleCollision();
private:
	Player* m_pPlayer;
	SoundEffect* m_pSpringSound;
};
