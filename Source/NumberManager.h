#pragma once

class SpriteManager;
class Texture;

class NumberManager final
{
public:
	explicit NumberManager(const Point2f& numberStartPos, const float& scale, const int& currentNumber = 0, const int& numbersToDraw = 1, bool reversePos = false);

	NumberManager(const NumberManager& other) = delete;
	NumberManager& operator=(const NumberManager& other) = delete;
	NumberManager(NumberManager&& other) = delete;
	NumberManager& operator=(NumberManager&& other) = delete;

	~NumberManager();

	void Reset(); // Resets the amount of numbers displayed back to the m_NumbersToDraw value

	void Update(const int& number, float& elapsedSec);
	void Draw() const;
private:
	bool m_ReversePos;

	float m_Scale;
	float m_SpriteHeight;
	float m_SpriteWidth;

	int m_CurrentNumber;
	int m_NumberSize;
	int m_NumbersToDraw;

	Point2f m_NumberStartPos;
	Texture* m_pNumbersTexture;

	std::vector<SpriteManager*> m_NumberSprites;

	static int SizeOfInt(int number);
	void AddNumber();
};
