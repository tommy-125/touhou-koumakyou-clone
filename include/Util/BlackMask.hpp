#ifndef UTIL_BLACK_MASK_HPP
#define UTIL_BLACK_MASK_HPP

#include <memory>

#include "Util/GameObject.hpp"

namespace Util {

class BlackMask {
   public:
    explicit BlackMask(float zIndex = 0.5f, float initAlpha = 0.0f);

    void Fade(int frames, float targetAlpha);
    void Update();

    std::shared_ptr<Util::GameObject> GetObj() { return m_Obj; }

   private:
    std::shared_ptr<Util::GameObject> m_Obj;
    float                             m_CurrentAlpha = 0.0f;
    float                             m_TargetAlpha  = 0.0f;
    float                             m_Step         = 0.0f;
};

}  // namespace Util

#endif
