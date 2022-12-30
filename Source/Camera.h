#pragma once

class Camera final
{
public:
	explicit Camera(const Window& window, const float& scale, const Point2f& center, const std::vector<Point2f>& bounds);

	Camera(const Camera& other) = delete;
	Camera& operator=(const Camera& other) = delete;
	Camera(Camera&& other) = delete;
	Camera& operator=(Camera&& other) = delete;

	~Camera() = default;

	std::vector<Point2f> m_LevelBounds;
	Vector2f m_CameraVelocity;

	Rectf GetCameraRect() const;
	void DrawDebug() const;
	void Transform() const;
	void Update(const Point2f& center, const float& deltaTime, const bool& isOnGround);

private:
	const float m_CameraLiftSpeed;
	float m_Height;
	float m_Scale;
	float m_Width;

	Point2f m_BottomLeftPos;
	Point2f m_PreviousPosition;
	Point2f m_TargetCenter;
	Rectf m_CameraArea;
	Rectf m_Target;

	void Clamp();
	void Track();
	void CalculateVelocity(const float& deltaTime);
};
