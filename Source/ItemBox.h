#pragma once
#include "GameObject.h"
class SoundEffect;
class Player;
class Rings;

class ItemBox final :
	public GameObject
{
public:
	explicit ItemBox(const std::string& ItemBoxPath, Player* player, Rings* rings);

	ItemBox(const ItemBox& other) = delete;
	ItemBox& operator=(const ItemBox& other) = delete;
	ItemBox(ItemBox&& other) = delete;
	ItemBox& operator=(ItemBox&& other) = delete;

	~ItemBox() override;

	virtual void Draw() const override;
	virtual void Update(float& elapsedSec, const float& fpsTarget);

	bool HandleCollision();
	virtual void Reset() override;

	void AddInvincibilityBoxLocations(const std::string& locationPath);
	void AddLifeBoxLocations(const std::string& locationPath);
	void AddRingBoxLocations(const std::string& locationPath);
private:
	Player* m_pPlayer;
	Rings* m_pRings;

	SoundEffect* m_pBoxDestroyedSound;

	std::vector<bool> m_Destroyed;
	std::vector<Point2f> m_ItemBoxInvincibilityLocations;
	std::vector<Point2f> m_ItemBoxLifeLocations;
	std::vector<Point2f> m_ItemBoxRingLocations;
	std::vector<SpriteManager*> m_ExplosionSprite;
	std::vector<SpriteManager*> m_ItemInvincibilitySprite;
	std::vector<SpriteManager*> m_ItemLifeSprite;
	std::vector<SpriteManager*> m_ItemRingSprite;

	Texture* m_pExplosionTexture;
	Texture* m_pItemBoxItemsTexture;

	std::vector<Point2f> Initialize(const std::string& spriteLocationsPath) const;
};
