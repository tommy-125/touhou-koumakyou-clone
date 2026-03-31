#include "Util/BlackMask.hpp"

#include "Util/Image.hpp"
#include "config.hpp"

namespace Util {

BlackMask::BlackMask(float zIndex, float initAlpha) : m_CurrentAlpha(initAlpha) {
    auto image               = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/black.png");
    m_Obj                    = std::make_shared<Util::GameObject>(image, zIndex);
    m_Obj->m_Transform.scale = {
        static_cast<float>(PTSD_Config::WINDOW_WIDTH) / image->GetSize().x,
        static_cast<float>(PTSD_Config::WINDOW_HEIGHT) / image->GetSize().y,
    };
    m_Obj->SetAlpha(initAlpha);
}

void BlackMask::Fade(int frames, float targetAlpha) {
    m_TargetAlpha = targetAlpha;
    m_Step        = (m_TargetAlpha - m_CurrentAlpha) / static_cast<float>(frames);
}

void BlackMask::Update() {
    if (m_Step == 0.0f) return;

    m_CurrentAlpha += m_Step;

    if (m_Step > 0.0f && m_CurrentAlpha >= m_TargetAlpha) {
        m_CurrentAlpha = m_TargetAlpha;
        m_Step         = 0.0f;
    } else if (m_Step < 0.0f && m_CurrentAlpha <= m_TargetAlpha) {
        m_CurrentAlpha = m_TargetAlpha;
        m_Step         = 0.0f;
    }

    m_Obj->SetAlpha(m_CurrentAlpha);
}

}  // namespace Util
