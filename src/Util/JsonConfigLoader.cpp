#include "Util/JsonConfigLoader.hpp"

#include <fstream>

namespace Util::JsonConfig {
std::string ResourcePath(const std::string& relativePath) {
    return std::string(GA_RESOURCE_DIR) + "/" + relativePath;
}

nlohmann::json LoadJsonFile(const std::string& path, const char* label) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error(std::string("failed to open ") + label + " JSON: " + path);
    }

    try {
        nlohmann::json root;
        file >> root;
        return root;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("failed to parse ") + label + " JSON " + path +
                                 ": " + e.what());
    }
}

glm::vec2 ParseVec2(const nlohmann::json& value, const char* fieldName) {
    if (!value.is_array() || value.size() != 2) {
        throw std::runtime_error(std::string(fieldName) + " must be a two-element array");
    }
    return {value.at(0).get<float>(), value.at(1).get<float>()};
}
}  // namespace Util::JsonConfig
