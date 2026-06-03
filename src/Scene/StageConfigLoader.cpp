#include "Scene/StageConfigLoader.hpp"

#include <unordered_map>

#include "Util/JsonConfigLoader.hpp"

namespace {
using StageConfigMap = std::unordered_map<std::string, StageConfig>;
namespace JsonConfig = Util::JsonConfig;

StageBackgroundConfig ParseBackgroundConfig(const nlohmann::json& json) {
    StageBackgroundConfig config;
    config.imagePath     = JsonConfig::ResourcePath(json.at("imagePath").get<std::string>());
    config.zIndex        = json.value("zIndex", config.zIndex);
    config.centerX       = json.value("centerX", config.centerX);
    config.canvasHeight  = json.at("canvasHeight").get<float>();
    config.fieldHeight   = json.value("fieldHeight", config.fieldHeight);
    return config;
}

StageConfig ParseStageConfig(const nlohmann::json& json) {
    StageConfig config;
    config.playable.timelinePath =
        JsonConfig::ResourcePath(json.at("timelinePath").get<std::string>());
    config.playable.stageNo               = json.at("stageNo").get<std::string>();
    config.playable.stageName             = json.at("stageName").get<std::string>();
    config.playable.songName              = json.at("songName").get<std::string>();
    config.playable.stageBgmPath          = json.at("stageBgmPath").get<std::string>();
    config.playable.bossBgmPath           = json.at("bossBgmPath").get<std::string>();
    config.playable.stageNameAdvanceScale =
        json.value("stageNameAdvanceScale", config.playable.stageNameAdvanceScale);
    config.playable.bossSkipFrame         = json.value("bossSkipFrame", -1);
    config.playable.midbossSkipFrame      = json.value("midbossSkipFrame", -1);
    config.playable.totalFrames           = json.at("totalFrames").get<int>();
    config.playable.stageBonus            = json.value("stageBonus", 0);
    config.playable.hasStageClear         = json.value("hasStageClear", false);
    config.playable.bossDeathResultDelay  = json.value("bossDeathResultDelay", 60);
    config.background                     = ParseBackgroundConfig(json.at("background"));
    return config;
}

StageConfigMap LoadStageConfigMap() {
    return JsonConfig::LoadConfigMap<StageConfig>("stages/stage_configs.json", "stage config",
                                                  ParseStageConfig);
}

}  // namespace

const StageConfig& LoadStageConfig(const std::string& stageId) {
    static const auto configs = LoadStageConfigMap();
    return JsonConfig::FindConfigRefOrThrow(configs, stageId, "stage");
}

std::unique_ptr<StageBackground> CreateStageBackground(Util::Renderer& renderer,
                                                       const StageConfig& config) {
    const auto& bg = config.background;
    return std::make_unique<LongScrollStageBackground>(
        renderer, bg.imagePath.c_str(), bg.zIndex, bg.centerX, bg.canvasHeight,
        bg.fieldHeight, config.playable.totalFrames);
}
