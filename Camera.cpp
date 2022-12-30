#include "pch.h"
#include "Camera.h"
#include <algorithm>
#include "utils.h"

Camera::Camera(const Window& window, const float& scale, const Point2f& center, const std::vector<Point2f>& bounds)
	: m_LevelBounds{ bounds }
	, m_CameraVelocity{ 0.0f, 0.0f }
	, m_CameraLiftSpeed{ 1.5f }
	, m_Height{ window.height / scale }
	, m_Scale{ scale }
	, m_Width{ window.width / scale }
	, m_BottomLeftPos{ 0.0f, 0.0f }
	, m_PreviousPosition{ 0.0f, 0.0f }
	, m_TargetCenter{ center }
	, m_CameraArea{ 0.0f, 0.0f, 16.0f, 64.0f }
{
	// We need to sort the vector in descending order so we can make assumptions in the clamp function
	std::sort(m_LevelBounds.begin(), m_LevelBounds.end(), [](Point2f a, Point2f b) { return a.x > b.x; });
}

void Camera::Transform() const
{
	glScalef(m_Scale, m_Scale, 1);
	glTranslatef(-m_BottomLeftPos.x, -m_BottomLeftPos.y, 1.0f);
}

Rectf Camera::GetCameraRect() const
{
	return Rectf{ m_BottomLeftPos.x, m_BottomLeftPos.y, m_Width, m_Height };
}

void Camera::DrawDebug() const
{
	utils::SetColor(Color4f{ 1.0f, 1.0f, 1.0f, 1.0f });
	utils::DrawRect(m_CameraArea, 3.0f);
}

void Camera::Update(const Point2f& center, const float& deltaTime, const bool& isOnGround)
{
	m_TargetCenter = center;
	CalculateVelocity(deltaTime);

	if (isOnGround) // Smooth lifting of camera to the center of the camera area while tracked object is on the ground
	{
		if (m_CameraArea.bottom > m_TargetCenter.y - m_CameraArea.height / 2) m_CameraArea.bottom -= m_CameraLiftSpeed * deltaTime;
		if (m_CameraArea.bottom < m_TargetCenter.y - m_CameraArea.height / 2) m_CameraArea.bottom += m_CameraLiftSpeed * deltaTime;
	}

	Track();
}

void Camera::Track()
{
	// Clamp the camera area x-position to the center of the tracked object when it leaves the area
	if (m_TargetCenter.x > m_CameraArea.left + m_CameraArea.width) m_CameraArea.left = m_TargetCenter.x - m_CameraArea.width;
	if (m_TargetCenter.x < m_CameraArea.left) m_CameraArea.left = m_TargetCenter.x;

	// Clamp the camera area y-position to the center of the tracked object when it leaves the area
	if (m_TargetCenter.y > m_CameraArea.bottom + m_CameraArea.height) m_CameraArea.bottom = m_TargetCenter.y - m_CameraArea.height;
	if (m_TargetCenter.y < m_CameraArea.bottom) m_CameraArea.bottom = m_TargetCenter.y;

	// Set the camera position based on the camera area
	m_BottomLeftPos.x = m_CameraArea.left - (m_Width - m_CameraArea.width) / 2;
	m_BottomLeftPos.y = m_CameraArea.bottom - (m_Height - m_CameraArea.height) / 2;

	Clamp();
}

void Camera::CalculateVelocity(const float& deltaTime)
{
	m_CameraVelocity.x = (m_BottomLeftPos.x - m_PreviousPosition.x) / deltaTime;
	m_CameraVelocity.y = (m_BottomLeftPos.y - m_PreviousPosition.y) / deltaTime;
	m_PreviousPosition = m_BottomLeftPos;
}

void Camera::Clamp()
{
	// We can assume the last and first element are the left and right most level bounds respectively because we sorted the vector in descending order
	if (m_BottomLeftPos.x < m_LevelBounds.back().x) m_BottomLeftPos.x = m_LevelBounds.back().x; // Left level clamp
	if (m_BottomLeftPos.x + m_Width > m_LevelBounds.front().x) m_BottomLeftPos.x = m_LevelBounds.front().x - m_Width; // Right level clamp

	for (const auto& i : m_LevelBounds) // Apply extra camera-y restrictions
	{
		if (m_BottomLeftPos.x > i.x)
		{
			if (m_BottomLeftPos.y < i.y) m_BottomLeftPos.y = i.y;
			// Because the vector is sorted in descending order the moment we find that the bottom left camera position
			// is greater than the x-position where the camera-y restriction should apply we can return from the function
			return;
		}
	}
	if (m_BottomLeftPos.y < m_LevelBounds.back().y) m_BottomLeftPos.y = m_LevelBounds.back().y; // If there are no restrictions applied in the for-loop apply the bottom most camera-y restriction
}
