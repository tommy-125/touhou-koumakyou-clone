#include "Scene/Stage6/Stage6.hpp"

#include <memory>

#include "Scene/StageConfigLoader.hpp"
#include "Scene/Stage6/Stage6Script.hpp"
#include "Scene/Title.hpp"

namespace {

const StageConfig& Stage6Config() {
    return LoadStageConfig("stage6");
}

}  // namespace

Stage6::Stage6(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, Stage6Config().playable,
                    std::make_unique<Stage6Script>()) {
    SetBackground(CreateStageBackground(m_Renderer, Stage6Config()));
}

std::unique_ptr<Scene> Stage6::NextScene() {
    return std::make_unique<Title>();
}
