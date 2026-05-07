#ifndef SCENE_STAGE_BACKGROUND_HPP
#define SCENE_STAGE_BACKGROUND_HPP

#include <array>
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

class Stage3CourtyardBackground : public StageBackground {
   public:
    Stage3CourtyardBackground(Util::Renderer& renderer, const char* spriteFolder, float zIndex);
    void Update(int frame) override;

   private:
    struct TileObj {
        std::shared_ptr<Util::GameObject> obj;
        int                               variant = 0;
        float                             baseX   = 0.0f;
        float                             baseY   = 0.0f;
    };

    struct CloudObj {
        std::shared_ptr<Util::GameObject> obj;
        float                             baseX = 0.0f;
        float                             baseY = 0.0f;
        float                             speed = 0.0f;
    };

    std::array<std::shared_ptr<Util::Image>, 4> m_TileImages;
    std::array<std::shared_ptr<Util::Image>, 2> m_CloudImages;
    std::vector<TileObj>                        m_Tiles;
    std::vector<CloudObj>                       m_Clouds;
};

#endif  // SCENE_STAGE_BACKGROUND_HPP
