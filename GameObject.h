#pragma once
#include "SpriteManager.h"

class Texture;

class GameObject
{
public:
	explicit GameObject(const std::vector<Point2f>& locations, const std::string& texturePath, const Vector2f& hitboxSize);
	explicit GameObject(const std::string& locationFilePath, const std::string& texturePath, const Vector2f& hitboxSize, float spriteWidth, float spriteHeight);
	explicit GameObject(const std::vector<Point2f>& locations, const std::string& texturePath, const Vector2f& hitboxSize, float spriteWidth, float spriteHeight);

	GameObject(const GameObject& other) = delete;
	GameObject& operator=(const GameObject& other) = delete;
	GameObject(GameObject&& other) = delete;
	GameObject& operator=(GameObject&& other) = delete;

	virtual ~GameObject();

	virtual void Draw() const;
	virtual void Update(float& elapsedSec, const float& fpsTarget);

	virtual void Reset();

	std::vector<Point2f> GetPositions() const;
protected:
	Texture* m_pTexture;

	const float m_SpriteHeight;
	const float m_SpriteWidth;
	const Vector2f m_HitBoxSize;

	float m_DeltaTime;

	std::vector<Rectf> m_HitBox;
	std::vector<Point2f> m_Locations;
	std::vector<SpriteManager*> m_pSprites;

private:
	std::vector<Point2f> GetPositionsFromFile(const std::string& locationFilePath);

	void InitGameObjects();
};
