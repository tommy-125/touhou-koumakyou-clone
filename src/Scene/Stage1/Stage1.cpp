#include "Scene/Stage1/Stage1.hpp"

#include <memory>

#include "Scene/StageConfigLoader.hpp"
#include "Scene/Stage1/Stage1Script.hpp"
#include "Scene/Stage2/Stage2.hpp"
#include "Scene/Title.hpp"

namespace {

const StageConfig& Stage1Config() {
    return LoadStageConfig("stage1");
}

}  // namespace

Stage1::Stage1(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, Stage1Config().playable,
                    std::make_unique<Stage1Script>()) {
    SetBackground(CreateStageBackground(m_Renderer, Stage1Config()));
}

std::unique_ptr<Scene> Stage1::NextScene() {
    if (auto cheatStage = CreateCheatStageScene()) return cheatStage;
    if (ShouldReturnToTitle() || WasGameOver()) return std::make_unique<Title>();
    return std::make_unique<Stage2>(m_Character, m_SpellCard, m_GameManager);
}
