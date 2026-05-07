#ifndef SCENE_STAGE_BACKGROUND_HPP
#define SCENE_STAGE_BACKGROUND_HPP

#include <array>
#include <memory>

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
    std::shared_ptr<Util::GameObject> m_Obj;
};

class TiledStageBackground : public StageBackground {
   public:
    TiledStageBackground(Util::Renderer& renderer, const char* imagePath, float zIndex,
                         float centerX, float scale, float tileSize, float scrollSpeed,
                         float swayAmplitude = 0.0f, float swayRate = 0.0f);
    void Update(int frame) override;

   private:
    float m_CenterX;
    float m_Scale;
    float m_TileHeight;
    float m_ScrollSpeed;
    float m_SwayAmplitude;
    float m_SwayRate;
    std::shared_ptr<Util::Image> m_Image;
    std::array<std::shared_ptr<Util::GameObject>, 3> m_Objs;
};

#endif  // SCENE_STAGE_BACKGROUND_HPP
