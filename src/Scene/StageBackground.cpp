#include "Scene/StageBackground.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

LongScrollStageBackground::LongScrollStageBackground(Util::Renderer& renderer,
                                                     const char* imagePath, float zIndex,
                                                     float centerX, float canvasHeight,
                                                     float fieldHeight, int totalFrames)
    : m_CenterX(centerX),
      m_CanvasHeight(canvasHeight),
      m_FieldHeight(fieldHeight),
      m_TotalFrames(totalFrames),
      m_Image(std::make_shared<Util::Image>(imagePath)) {
    constexpr int kTileCount = 2;
    m_Objs.reserve(kTileCount);
    for (int i = 0; i < kTileCount; ++i) {
        auto obj = std::make_shared<Util::GameObject>(m_Image, zIndex);
        renderer.AddChild(obj);
        m_Objs.push_back(std::move(obj));
    }
    Update(0);
}

void LongScrollStageBackground::Update(int frame) {
    if (m_Objs.empty()) return;

    const float scrollRange = std::max(0.0f, m_CanvasHeight - m_FieldHeight);
    const float speed = m_TotalFrames > 0 ? scrollRange / static_cast<float>(m_TotalFrames)
                                          : 0.0f;
    const float rawScrollY = static_cast<float>(std::max(0, frame)) * speed;
    const float scrollY = m_CanvasHeight > 0.0f ? std::fmod(rawScrollY, m_CanvasHeight) : 0.0f;
    const float topAlignedY = m_CanvasHeight / 2.0f - m_FieldHeight / 2.0f;

    for (std::size_t i = 0; i < m_Objs.size(); ++i) {
        m_Objs[i]->m_Transform.translation = {
            m_CenterX,
            topAlignedY - scrollY + static_cast<float>(i) * m_CanvasHeight,
        };
    }
}
