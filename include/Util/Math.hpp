#ifndef UTIL_MATH_HPP
#define UTIL_MATH_HPP

#include <glm/glm.hpp>

namespace Util {
constexpr float HALF_PI = 1.5707964f;

// Game field to screen coordinate offset
constexpr float FIELD_OFFSET_X = 32.0f;
constexpr float FIELD_OFFSET_Y = 16.0f;

constexpr float GAME_BOUNDS_LEFT   = 32.0f;
constexpr float GAME_BOUNDS_RIGHT  = 416.0f;
constexpr float GAME_BOUNDS_TOP    = 16.0f;
constexpr float GAME_BOUNDS_BOTTOM = 464.0f;

glm::vec2 GameFieldToScreen(float x, float y);
glm::vec2 GameFieldToScreen(glm::vec2 pos);

bool IsInGameBounds(float x, float y, float width, float height,
                    float boundsLeft = GAME_BOUNDS_LEFT, float boundsTop = GAME_BOUNDS_TOP,
                    float boundsRight = GAME_BOUNDS_RIGHT, float boundsBottom = GAME_BOUNDS_BOTTOM);
}  // namespace Util

#endif  // UTIL_MATH_HPP