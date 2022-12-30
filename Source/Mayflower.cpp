#include "pch.h"
#include "Mayflower.h"

Mayflower::Mayflower(const std::string& locationFilePath)
	: GameObject{ locationFilePath, "./Resources/Sprites/Game Objects/Mayflowers_Texture.png", Vector2f{}, 32.0f, 40.0f }
{
	for (const auto& i : m_pSprites) i->ChangeSprite(1, 1, 1, 4, 1);
}
