#include "pch.h"
#include "Waterfall.h"

#include "Player.h"

Waterfall::Waterfall(const std::string& locationFilePath)
	: GameObject{ locationFilePath, "./Resources/Sprites/Game Objects/Waterfall_Texture.png", Vector2f{}, 192.0f, 384.0f }
{
	for (const auto& i : m_pSprites) i->ChangeSprite(1, 1, 1, 4, 10);
}
