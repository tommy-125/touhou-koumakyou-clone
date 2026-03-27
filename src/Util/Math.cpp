#include "Util/Math.hpp"

namespace Util {
bool IsInGameBounds(float x, float y, float width, float height, float boundsLeft, float boundsTop, float boundsRight, float boundsBottom) {
    return (x + width > boundsLeft) && (x - width < boundsRight) &&
           (y + height > boundsTop) && (y - height < boundsBottom);
}
} // namespace Util