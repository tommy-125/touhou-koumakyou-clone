#include "Scene/StageScriptUtil.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "Anm/AnmDefs.hpp"

namespace {
using EnemyInitConfigMap = std::unordered_map<std::string, StageScriptUtil::EnemyInitConfig>;
using BossEntryConfigMap = std::unordered_map<std::string, StageScriptUtil::BossEntryConfig>;
using BossPhaseConfigMap = std::unordered_map<std::string, BossPhaseUtil::PhaseConfig>;
using MovementProfileMap = std::unordered_map<std::string, StageScriptUtil::MovementProfile>;
using RewardConfigMap    = std::unordered_map<std::string, StageScriptUtil::RewardConfig>;

int AnmBaseFromName(const std::string& name) {
    if (name == "STG1ENM") return Anm::STG1ENM.offset;
    if (name == "STG2ENM") return Anm::STG2ENM.offset;
    if (name == "STG3ENM") return Anm::STG3ENM.offset;
    if (name == "STG4ENM") return Anm::STG4ENM.offset;
    if (name == "STG5ENM") return Anm::STG5ENM.offset;
    if (name == "STG5ENM2") return Anm::STG5ENM2.offset;
    if (name == "STG6ENM") return Anm::STG6ENM.offset;
    if (name == "STG6ENM2") return Anm::STG6ENM2.offset;
    throw std::runtime_error("unknown ANM base: " + name);
}

glm::vec2 ParseVec2(const nlohmann::json& value, const char* fieldName) {
    if (!value.is_array() || value.size() != 2) {
        throw std::runtime_error(std::string(fieldName) + " must be a two-element array");
    }
    return {value.at(0).get<float>(), value.at(1).get<float>()};
}

template <typename T>
void ReadOptional(const nlohmann::json& item, const char* key, std::optional<T>& output) {
    if (item.contains(key)) output = item.at(key).get<T>();
}

template <typename Config, typename ParseFn>
std::unordered_map<std::string, Config> LoadConfigMap(const char* path, const char* label,
                                                      ParseFn parse) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error(std::string("failed to open ") + label + " JSON: " + path);
    }

    try {
        nlohmann::json root;
        file >> root;
        if (!root.is_object()) {
            throw std::runtime_error("root must be an object keyed by config id");
        }

        std::unordered_map<std::string, Config> configs;
        configs.reserve(root.size());
        for (const auto& [id, value] : root.items()) {
            try {
                configs.emplace(id, parse(value));
            } catch (const std::exception& e) {
                throw std::runtime_error("entry '" + id + "': " + e.what());
            }
        }
        return configs;
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("failed to parse ") + label + " JSON " + path +
                                 ": " + e.what());
    }
}

template <typename ConfigMap>
typename ConfigMap::mapped_type FindConfigOrDefault(const ConfigMap& configs, const std::string& id,
                                                    const char* label) {
    const auto it = configs.find(id);
    if (it == configs.end()) {
        throw std::runtime_error(std::string("missing ") + label + " config: " + id);
    }
    return it->second;
}

StageScriptUtil::EnemyInitConfig ParseEnemyInitConfig(const nlohmann::json& item) {
    StageScriptUtil::EnemyInitConfig config;
    config.visual.anmBase = AnmBaseFromName(item.at("anm").get<std::string>());
    config.visual.script  = item.at("script").get<int>();
    config.visual.hitbox  = ParseVec2(item.at("hitbox"), "hitbox");

    if (item.contains("deathEffect")) {
        const glm::vec2 effect             = ParseVec2(item.at("deathEffect"), "deathEffect");
        config.visual.deathEffectPrimary   = static_cast<int>(effect.x);
        config.visual.deathEffectSecondary = static_cast<int>(effect.y);
    }

    ReadOptional(item, "angle", config.angle);
    ReadOptional(item, "speed", config.speed);
    ReadOptional(item, "acceleration", config.acceleration);
    ReadOptional(item, "angularVelocity", config.angularVelocity);
    ReadOptional(item, "itemDrop", config.itemDrop);
    ReadOptional(item, "itemDropCount", config.itemDropCount);
    ReadOptional(item, "deathCallbackSub", config.deathCallbackSub);
    ReadOptional(item, "canTakeDamage", config.canTakeDamage);
    ReadOptional(item, "rotateWithAngle", config.rotateWithAngle);
    return config;
}

StageScriptUtil::BossEntryConfig ParseBossEntryConfig(const nlohmann::json& item) {
    StageScriptUtil::BossEntryConfig config;
    config.anmBase        = AnmBaseFromName(item.at("anm").get<std::string>());
    config.script         = item.at("script").get<int>();
    config.fieldPos       = ParseVec2(item.at("fieldPos"), "fieldPos");
    config.hitbox         = ParseVec2(item.at("hitbox"), "hitbox");
    config.title          = item.value("title", "");
    config.blocksTimeline = item.value("blocksTimeline", config.blocksTimeline);
    config.lifeCount      = item.value("lifeCount", config.lifeCount);
    ReadOptional(item, "itemDrop", config.itemDrop);

    if (item.contains("boundsMin")) {
        config.boundsMinField = ParseVec2(item.at("boundsMin"), "boundsMin");
    }
    if (item.contains("boundsMax")) {
        config.boundsMaxField = ParseVec2(item.at("boundsMax"), "boundsMax");
    }

    if (item.contains("deathEffect")) {
        const glm::vec2 effect      = ParseVec2(item.at("deathEffect"), "deathEffect");
        config.deathEffectPrimary   = static_cast<int>(effect.x);
        config.deathEffectSecondary = static_cast<int>(effect.y);
    }
    return config;
}

BossPhaseUtil::PhaseConfig ParseBossPhaseConfig(const nlohmann::json& item) {
    BossPhaseUtil::PhaseConfig config;
    config.title         = item.value("title", "");
    config.life          = item.value("life", config.life);
    config.lifeCount     = item.value("lifeCount", config.lifeCount);
    config.timerFrames   = item.value("timerFrames", config.timerFrames);
    config.timerSub      = item.value("timerSub", config.timerSub);
    config.deathSub      = item.value("deathSub", config.deathSub);
    config.lifeThreshold = item.value("lifeThreshold", config.lifeThreshold);
    config.lifeSub       = item.value("lifeSub", config.lifeSub);
    config.spell         = item.value("spell", config.spell);
    config.showSpellName = item.value("showSpellName", config.showSpellName);
    config.spellBonus    = item.value("spellBonus", config.spellBonus);
    config.canTakeDamage = item.value("canTakeDamage", config.canTakeDamage);
    config.cancelBullets = item.value("cancelBullets", config.cancelBullets);
    return config;
}

StageScriptUtil::MovementProfile ParseMovementProfile(const nlohmann::json& item) {
    StageScriptUtil::MovementProfile profile;
    if (!item.contains("events") || !item.at("events").is_array()) {
        throw std::runtime_error("events must be an array");
    }
    for (const auto& value : item.at("events")) {
        StageScriptUtil::MovementEvent event;
        event.time = value.at("time").get<int>();
        ReadOptional(value, "angle", event.angle);
        ReadOptional(value, "mirroredAngle", event.mirroredAngle);
        ReadOptional(value, "speed", event.speed);
        ReadOptional(value, "acceleration", event.acceleration);
        ReadOptional(value, "angularVelocity", event.angularVelocity);
        ReadOptional(value, "mirroredAngularVelocity", event.mirroredAngularVelocity);
        profile.events.push_back(event);
    }
    return profile;
}

ItemType ItemTypeFromName(const std::string& name) {
    if (name == "PowerSmall") return ItemType::PowerSmall;
    if (name == "Point") return ItemType::Point;
    if (name == "PowerBig") return ItemType::PowerBig;
    if (name == "Bomb") return ItemType::Bomb;
    if (name == "FullPower") return ItemType::FullPower;
    if (name == "Life") return ItemType::Life;
    if (name == "PointBullet") return ItemType::PointBullet;
    throw std::runtime_error("unknown reward item type: " + name);
}

StageScriptUtil::RewardConfig ParseRewardConfig(const nlohmann::json& item) {
    StageScriptUtil::RewardConfig config;
    config.power                = item.value("power", config.power);
    config.bulletCancelToPoints = item.value("bulletCancelToPoints", config.bulletCancelToPoints);
    config.die                  = item.value("die", config.die);

    if (item.contains("items")) {
        for (const auto& value : item.at("items")) {
            config.items.push_back(ItemTypeFromName(value.get<std::string>()));
        }
    }
    return config;
}

const EnemyInitConfigMap& EnemyInitConfigs() {
    static const EnemyInitConfigMap configs = LoadConfigMap<StageScriptUtil::EnemyInitConfig>(
        GA_RESOURCE_DIR "/stages/enemy_init_configs.json", "enemy init config",
        ParseEnemyInitConfig);
    return configs;
}

const BossEntryConfigMap& BossEntryConfigs() {
    static const BossEntryConfigMap configs = LoadConfigMap<StageScriptUtil::BossEntryConfig>(
        GA_RESOURCE_DIR "/stages/boss_entry_configs.json", "boss entry config",
        ParseBossEntryConfig);
    return configs;
}

const BossPhaseConfigMap& BossPhaseConfigs() {
    static const BossPhaseConfigMap configs =
        LoadConfigMap<BossPhaseUtil::PhaseConfig>(GA_RESOURCE_DIR "/stages/boss_phase_configs.json",
                                                  "boss phase config", ParseBossPhaseConfig);
    return configs;
}

const RewardConfigMap& RewardConfigs() {
    static const RewardConfigMap configs = LoadConfigMap<StageScriptUtil::RewardConfig>(
        GA_RESOURCE_DIR "/stages/reward_configs.json", "reward config", ParseRewardConfig);
    return configs;
}

const MovementProfileMap& MovementProfiles() {
    static const MovementProfileMap configs = LoadConfigMap<StageScriptUtil::MovementProfile>(
        GA_RESOURCE_DIR "/stages/movement_profiles.json", "movement profile",
        ParseMovementProfile);
    return configs;
}
}  // namespace

namespace StageScriptUtil {
EnemyInitConfig LoadEnemyInitConfig(const std::string& id) {
    return FindConfigOrDefault(EnemyInitConfigs(), id, "enemy init");
}

BossEntryConfig LoadBossEntryConfig(const std::string& id) {
    return FindConfigOrDefault(BossEntryConfigs(), id, "boss entry");
}

BossPhaseUtil::PhaseConfig LoadBossPhaseConfig(const std::string& id) {
    return FindConfigOrDefault(BossPhaseConfigs(), id, "boss phase");
}

RewardConfig LoadRewardConfig(const std::string& id) {
    return FindConfigOrDefault(RewardConfigs(), id, "reward");
}

MovementProfile LoadMovementProfile(const std::string& id) {
    return FindConfigOrDefault(MovementProfiles(), id, "movement profile");
}

void ApplyMovementProfile(Enemy& enemy, const MovementProfile& profile, int time) {
    for (const MovementEvent& event : profile.events) {
        if (event.time != time) continue;
        if (event.angle || event.mirroredAngle) {
            const auto& angle = enemy.m_Mirrored && event.mirroredAngle ? event.mirroredAngle
                                                                        : event.angle;
            if (angle) enemy.m_Angle = *angle;
        }
        if (event.speed) enemy.m_Speed = *event.speed;
        if (event.acceleration) enemy.m_Acceleration = *event.acceleration;
        if (event.angularVelocity || event.mirroredAngularVelocity) {
            const auto& angularVelocity = enemy.m_Mirrored && event.mirroredAngularVelocity
                                               ? event.mirroredAngularVelocity
                                               : event.angularVelocity;
            if (angularVelocity) enemy.m_AngularVelocity = *angularVelocity;
        }
    }
}

void ApplyMovementProfile(Enemy& enemy, const std::string& id, int time) {
    const auto& configs = MovementProfiles();
    const auto  it      = configs.find(id);
    if (it == configs.end()) {
        throw std::runtime_error("missing movement profile config: " + id);
    }
    ApplyMovementProfile(enemy, it->second, time);
}

void ApplyReward(Enemy& enemy, EnemySubCtx& ctx, const RewardConfig& config) {
    if (config.bulletCancelToPoints) ctx.BulletCancelIntoPointItems();
    if (config.power > 0) EnemyScriptUtil::DropPowerItems(enemy, ctx, config.power);
    for (ItemType item : config.items) {
        ctx.items.SpawnItem(enemy.m_Pos, item);
    }
    if (config.die) enemy.m_Alive = false;
}

void ApplyReward(Enemy& enemy, EnemySubCtx& ctx, const std::string& id) {
    ApplyReward(enemy, ctx, LoadRewardConfig(id));
}
}  // namespace StageScriptUtil
