#include "Scene/TimelineLoader.hpp"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace {

TimelineEntry ParseTimelineEntry(const nlohmann::json& item) {
    TimelineEntry entry;
    entry.frame    = item.at("frame").get<int>();
    entry.subId    = item.at("subId").get<int>();
    entry.x        = item.at("x").get<float>();
    entry.y        = item.at("y").get<float>();
    entry.life     = item.at("life").get<int>();
    entry.score    = item.at("score").get<int>();
    entry.mirrored = item.value("mirrored", false);
    entry.randomX  = item.value("randomX", false);
    entry.itemDrop = item.value("itemDrop", -99);
    return entry;
}

}  // namespace

std::vector<TimelineEntry> LoadTimelineFromJson(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("failed to open timeline JSON: " + path);
    }

    try {
        nlohmann::json root;
        file >> root;

        const auto& timelineJson = root.at("timeline");
        if (!timelineJson.is_array()) {
            throw std::runtime_error("timeline field is not an array");
        }

        std::vector<TimelineEntry> entries;
        entries.reserve(timelineJson.size());
        for (const auto& item : timelineJson) {
            entries.push_back(ParseTimelineEntry(item));
        }
        return entries;
    } catch (const std::exception& e) {
        throw std::runtime_error("failed to parse timeline JSON " + path + ": " + e.what());
    }
}
