#include "pch.h"
#include "Sunflower.h"

Sunflower::Sunflower(const std::string& locationFilePath)
	: GameObject{ locationFilePath, "./Resources/Sprites/Game Objects/Sunflowers_Texture.png", Vector2f{}, 32.0f, 32.0f }
{
	for (const auto& i : m_pSprites) i->ChangeSprite(1, 1, 1, 2, 1);
}
