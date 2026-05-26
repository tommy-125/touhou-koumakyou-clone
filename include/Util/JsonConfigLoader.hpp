#ifndef UTIL_JSON_CONFIG_LOADER_HPP
#define UTIL_JSON_CONFIG_LOADER_HPP

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

namespace Util::JsonConfig {
std::string    ResourcePath(const std::string& relativePath);
nlohmann::json LoadJsonFile(const std::string& path, const char* label);
glm::vec2      ParseVec2(const nlohmann::json& value, const char* fieldName);

template <typename T>
void ReadOptional(const nlohmann::json& item, const char* key, std::optional<T>& output) {
    if (item.contains(key)) output = item.at(key).get<T>();
}

template <typename Config, typename ParseFn>
std::unordered_map<std::string, Config> LoadConfigMap(const std::string& relativePath,
                                                      const char* label, ParseFn parse) {
    const std::string path = ResourcePath(relativePath);
    const auto        root = LoadJsonFile(path, label);
    if (!root.is_object()) {
        throw std::runtime_error(std::string(label) +
                                 " JSON root must be an object keyed by config id: " + path);
    }

    std::unordered_map<std::string, Config> configs;
    configs.reserve(root.size());
    for (const auto& [id, value] : root.items()) {
        try {
            configs.emplace(id, parse(value));
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string(label) + " config '" + id +
                                     "' is invalid: " + e.what());
        }
    }
    return configs;
}

template <typename ConfigMap>
const typename ConfigMap::mapped_type& FindConfigRefOrThrow(const ConfigMap& configs,
                                                            const std::string& id,
                                                            const char* label) {
    const auto it = configs.find(id);
    if (it == configs.end()) {
        throw std::runtime_error(std::string("missing ") + label + " config: " + id);
    }
    return it->second;
}

template <typename ConfigMap>
typename ConfigMap::mapped_type FindConfigOrThrow(const ConfigMap& configs,
                                                  const std::string& id,
                                                  const char* label) {
    return FindConfigRefOrThrow(configs, id, label);
}
}  // namespace Util::JsonConfig

#endif  // UTIL_JSON_CONFIG_LOADER_HPP
