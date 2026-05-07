#include "Scene/StageBackground.hpp"

#include <cmath>

LongScrollStageBackground::LongScrollStageBackground(Util::Renderer& renderer,
                                                     const char* imagePath, float zIndex,
                                                     float centerX, float canvasHeight,
                                                     float fieldHeight, int totalFrames)
    : m_CenterX(centerX),
      m_CanvasHeight(canvasHeight),
      m_FieldHeight(fieldHeight),
      m_TotalFrames(totalFrames),
      m_Image(std::make_shared<Util::Image>(imagePath)),
      m_Obj(std::make_shared<Util::GameObject>(m_Image, zIndex)) {
    renderer.AddChild(m_Obj);
    Update(0);
}

void LongScrollStageBackground::Update(int frame) {
    if (!m_Obj) return;

    const float scrollY =
        static_cast<float>(frame) * (m_CanvasHeight - m_FieldHeight) /
        static_cast<float>(m_TotalFrames);
    m_Obj->m_Transform.translation = {
        m_CenterX,
        (m_CanvasHeight / 2.0f - m_FieldHeight / 2.0f) - scrollY,
    };
}

TiledStageBackground::TiledStageBackground(Util::Renderer& renderer, const char* imagePath,
                                           float zIndex, float centerX, float scale,
                                           float tileSize, float scrollSpeed,
                                           float swayAmplitude, float swayRate)
    : m_CenterX(centerX),
      m_Scale(scale),
      m_TileHeight(tileSize * scale),
      m_ScrollSpeed(scrollSpeed),
      m_SwayAmplitude(swayAmplitude),
      m_SwayRate(swayRate),
      m_Image(std::make_shared<Util::Image>(imagePath)) {
    for (auto& obj : m_Objs) {
        obj                    = std::make_shared<Util::GameObject>(m_Image, zIndex);
        obj->m_Transform.scale = {m_Scale, m_Scale};
        renderer.AddChild(obj);
    }
    Update(0);
}

void TiledStageBackground::Update(int frame) {
    const float scroll = std::fmod(static_cast<float>(frame) * m_ScrollSpeed, m_TileHeight);
    const float sway   = m_SwayAmplitude * std::sin(static_cast<float>(frame) * m_SwayRate);

    for (size_t i = 0; i < m_Objs.size(); i++) {
        auto& obj = m_Objs[i];
        if (!obj) continue;
        const float tileOffset = (static_cast<float>(i) - 1.0f) * m_TileHeight;
        obj->m_Transform.translation = {m_CenterX + sway, tileOffset - scroll};
    }
}
