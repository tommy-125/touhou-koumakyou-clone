#include "Scene/StageConfigLoader.hpp"

#include <fstream>
#include <iostream>
#include <map>

#include <nlohmann/json.hpp>

namespace {

std::string ResourcePath(const std::string& relativePath) {
    return std::string(GA_RESOURCE_DIR) + "/" + relativePath;
}

StageBackgroundConfig ParseBackgroundConfig(const nlohmann::json& json) {
    StageBackgroundConfig config;
    config.imagePath     = ResourcePath(json.at("imagePath").get<std::string>());
    config.zIndex        = json.value("zIndex", config.zIndex);
    config.centerX       = json.value("centerX", config.centerX);
    config.canvasHeight  = json.at("canvasHeight").get<float>();
    config.fieldHeight   = json.value("fieldHeight", config.fieldHeight);
    return config;
}

StageConfig ParseStageConfig(const nlohmann::json& json) {
    StageConfig config;
    config.playable.timelinePath          = ResourcePath(json.at("timelinePath").get<std::string>());
    config.playable.stageNo               = json.at("stageNo").get<std::string>();
    config.playable.stageName             = json.at("stageName").get<std::string>();
    config.playable.songName              = json.at("songName").get<std::string>();
    config.playable.bossSkipFrame         = json.value("bossSkipFrame", -1);
    config.playable.midbossSkipFrame      = json.value("midbossSkipFrame", -1);
    config.playable.totalFrames           = json.at("totalFrames").get<int>();
    config.playable.stageBonus            = json.value("stageBonus", 0);
    config.playable.hasStageClear         = json.value("hasStageClear", false);
    config.playable.bossDeathResultDelay  = json.value("bossDeathResultDelay", 60);
    config.background                     = ParseBackgroundConfig(json.at("background"));
    return config;
}

std::map<std::string, StageConfig> LoadStageConfigMap() {
    const std::string path = ResourcePath("stages/stage_configs.json");
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open stage config JSON: " << path << '\n';
        return {};
    }

    try {
        nlohmann::json root;
        file >> root;

        std::map<std::string, StageConfig> configs;
        for (const auto& [stageId, json] : root.items()) {
            configs.emplace(stageId, ParseStageConfig(json));
        }
        return configs;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse stage config JSON " << path << ": " << e.what()
                  << '\n';
        return {};
    }
}

}  // namespace

const StageConfig& LoadStageConfig(const std::string& stageId) {
    static const auto configs = LoadStageConfigMap();
    const auto        it      = configs.find(stageId);
    if (it != configs.end()) return it->second;

    std::cerr << "Missing stage config: " << stageId << '\n';
    static const StageConfig emptyConfig;
    return emptyConfig;
}

std::unique_ptr<StageBackground> CreateStageBackground(Util::Renderer& renderer,
                                                       const StageConfig& config) {
    const auto& bg = config.background;
    return std::make_unique<LongScrollStageBackground>(
        renderer, bg.imagePath.c_str(), bg.zIndex, bg.centerX, bg.canvasHeight,
        bg.fieldHeight, config.playable.totalFrames);
}
