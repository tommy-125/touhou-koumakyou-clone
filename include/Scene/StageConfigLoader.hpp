#ifndef SCENE_STAGE_CONFIG_LOADER_HPP
#define SCENE_STAGE_CONFIG_LOADER_HPP

#include <memory>
#include <string>

#include "Scene/PlayableStage.hpp"
#include "Scene/StageBackground.hpp"

namespace Util {
class Renderer;
}

struct StageBackgroundConfig {
    std::string imagePath;
    float       zIndex       = -10.0f;
    float       centerX      = -96.0f;
    float       canvasHeight = 4096.0f;
    float       fieldHeight  = 448.0f;
};

struct StageConfig {
    PlayableStageConfig  playable;
    StageBackgroundConfig background;
};

const StageConfig& LoadStageConfig(const std::string& stageId);
std::unique_ptr<StageBackground> CreateStageBackground(Util::Renderer& renderer,
                                                       const StageConfig& config);

#endif  // SCENE_STAGE_CONFIG_LOADER_HPP
