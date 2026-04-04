#include "Util/Math.hpp"

namespace Util {
bool IsInGameBounds(float x, float y, float width, float height, float boundsLeft, float boundsTop,
                    float boundsRight, float boundsBottom) {
    return (x + width > boundsLeft) && (x - width < boundsRight) && (y + height > boundsTop) &&
           (y - height < boundsBottom);
}

glm::vec2 GameFieldToScreen(float x, float y) {
    return {x + FIELD_OFFSET_X, y + FIELD_OFFSET_Y};
}

glm::vec2 GameFieldToScreen(glm::vec2 pos) {
    return {pos.x + FIELD_OFFSET_X, pos.y + FIELD_OFFSET_Y};
}
}  // namespace Util