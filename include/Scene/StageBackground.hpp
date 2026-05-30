#ifndef SCENE_STAGE_BACKGROUND_HPP
#define SCENE_STAGE_BACKGROUND_HPP

#include <memory>
#include <vector>

#include "Util/GameObject.hpp"
#include "Util/Image.hpp"
#include "Util/Renderer.hpp"

class StageBackground {
   public:
    virtual ~StageBackground() = default;
    virtual void Update(int frame) = 0;
};

class LongScrollStageBackground : public StageBackground {
   public:
    LongScrollStageBackground(Util::Renderer& renderer, const char* imagePath, float zIndex,
                              float centerX, float canvasHeight, float fieldHeight,
                              int totalFrames);
    void Update(int frame) override;

   private:
    float m_CenterX;
    float m_CanvasHeight;
    float m_FieldHeight;
    int   m_TotalFrames;
    std::shared_ptr<Util::Image>      m_Image;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;
};

#endif  // SCENE_STAGE_BACKGROUND_HPP
