#ifndef UTIL_MATH_HPP
#define UTIL_MATH_HPP

#include <glm/glm.hpp>

namespace Util {
constexpr float GAME_BOUNDS_LEFT   = 32.0f;
constexpr float GAME_BOUNDS_RIGHT  = 416.0f;
constexpr float GAME_BOUNDS_TOP    = 16.0f;
constexpr float GAME_BOUNDS_BOTTOM = 464.0f;

bool IsInGameBounds(float x, float y, float width, float height,
                    float boundsLeft = GAME_BOUNDS_LEFT, float boundsTop = GAME_BOUNDS_TOP,
                    float boundsRight = GAME_BOUNDS_RIGHT, float boundsBottom = GAME_BOUNDS_BOTTOM);
}

#endif // UTIL_MATH_HPP