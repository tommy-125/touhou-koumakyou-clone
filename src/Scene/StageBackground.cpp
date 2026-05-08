#include "Scene/StageBackground.hpp"

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
