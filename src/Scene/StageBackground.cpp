#include "Scene/StageBackground.hpp"

#include <cmath>
#include <string>

namespace {
constexpr float TILE_OVERLAP = 2.0f;

std::shared_ptr<Util::Image> LoadSprite(const char* folder, int idx) {
    return std::make_shared<Util::Image>(std::string(folder) + "/sprite_" + std::to_string(idx) +
                                         ".png");
}

float WrapCentered(float y, float span) {
    return std::fmod(y + span * 0.5f + span * 100.0f, span) - span * 0.5f;
}
}

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
    const float tileStride = m_TileHeight - TILE_OVERLAP;
    const float scroll = std::fmod(static_cast<float>(frame) * m_ScrollSpeed, tileStride);
    const float sway   = m_SwayAmplitude * std::sin(static_cast<float>(frame) * m_SwayRate);

    for (size_t i = 0; i < m_Objs.size(); i++) {
        auto& obj = m_Objs[i];
        if (!obj) continue;
        const float tileOffset = (static_cast<float>(i) - 1.0f) * tileStride;
        obj->m_Transform.translation = {m_CenterX + sway, tileOffset - scroll};
    }
}

Stage3CourtyardBackground::Stage3CourtyardBackground(Util::Renderer& renderer,
                                                     const char* spriteFolder, float zIndex) {
    m_TileImages = {
        LoadSprite(spriteFolder, 3),  // red wall block
        LoadSprite(spriteFolder, 0),  // blue floor block
        LoadSprite(spriteFolder, 1),  // yellow worn block
        LoadSprite(spriteFolder, 2),  // gray stone block
    };
    m_CloudImages = {
        LoadSprite(spriteFolder, 10),
        LoadSprite(spriteFolder, 11),
    };

    static constexpr int   COLS      = 11;
    static constexpr int   ROWS      = 14;
    static constexpr float TILE_SIZE = 48.0f;
    static constexpr float CENTER_X  = -96.0f;

    m_Tiles.reserve(COLS * ROWS);
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int variant = 1;
            if (col <= 1 || col >= COLS - 2) {
                variant = 0;
            } else if ((row + col) % 7 == 0) {
                variant = 2;
            } else if ((row * 2 + col) % 9 == 0) {
                variant = 3;
            }

            auto obj = std::make_shared<Util::GameObject>(m_TileImages[variant], zIndex);
            obj->m_Transform.scale = {1.55f, 1.55f};
            renderer.AddChild(obj);

            m_Tiles.push_back({
                obj,
                variant,
                CENTER_X + (static_cast<float>(col) - (static_cast<float>(COLS) - 1.0f) * 0.5f) *
                               TILE_SIZE,
                (static_cast<float>(row) - (static_cast<float>(ROWS) - 1.0f) * 0.5f) * TILE_SIZE,
            });
        }
    }

    for (int i = 0; i < 6; i++) {
        auto obj = std::make_shared<Util::GameObject>(m_CloudImages[i % 2], zIndex + 0.5f);
        obj->m_Transform.scale = {1.6f + static_cast<float>(i % 3) * 0.25f,
                                  1.4f + static_cast<float>(i % 2) * 0.2f};
        obj->SetAlpha(0.45f);
        renderer.AddChild(obj);
        m_Clouds.push_back({
            obj,
            -250.0f + static_cast<float>(i) * 78.0f,
            -260.0f + static_cast<float>((i * 91) % 520),
            0.18f + static_cast<float>(i % 3) * 0.05f,
        });
    }

    Update(0);
}

void Stage3CourtyardBackground::Update(int frame) {
    static constexpr float TILE_SPAN    = 48.0f * 14.0f;
    static constexpr float TILE_SCROLL  = 0.55f;
    static constexpr float CLOUD_SPAN   = 640.0f;
    static constexpr float CLOUD_SCROLL = 0.18f;

    const float scroll = static_cast<float>(frame) * TILE_SCROLL;
    const float sway   = 1.5f * std::sin(static_cast<float>(frame) * 0.008f);

    for (auto& tile : m_Tiles) {
        if (!tile.obj) continue;
        tile.obj->m_Transform.translation = {
            tile.baseX + sway * (tile.variant == 0 ? 0.4f : 1.0f),
            WrapCentered(tile.baseY - scroll, TILE_SPAN),
        };
    }

    for (auto& cloud : m_Clouds) {
        if (!cloud.obj) continue;
        cloud.obj->m_Transform.translation = {
            cloud.baseX + 10.0f * std::sin(static_cast<float>(frame) * 0.006f + cloud.speed * 12.0f),
            WrapCentered(cloud.baseY - static_cast<float>(frame) * (CLOUD_SCROLL + cloud.speed),
                         CLOUD_SPAN),
        };
    }
}
