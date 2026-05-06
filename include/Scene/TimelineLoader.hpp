#ifndef SCENE_TIMELINE_LOADER_HPP
#define SCENE_TIMELINE_LOADER_HPP

#include <string>
#include <vector>

#include "Scene/Timeline.hpp"

std::vector<TimelineEntry> LoadTimelineFromJson(const std::string& path);

#endif  // SCENE_TIMELINE_LOADER_HPP
