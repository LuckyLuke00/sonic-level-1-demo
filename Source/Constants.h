#pragma once
#include "Vector2f.h"

// Player speed constants
const float acc{ 0.046875f };				// Acceleration
const float dec{ 0.5f };					// Deceleration
const float frc{ 0.046875f };				// Friction

const Vector2f top{ 6.0f , 16.0f };			// Top Speed

const float jmp{ 6.5f };					// Jump force
const float maxJmp{ jmp / 2.5f };			// Max Jump velocity
const float grv{ 0.21875f };				// Gravity
