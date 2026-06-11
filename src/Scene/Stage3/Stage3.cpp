#include "Scene/Stage3/Stage3.hpp"

#include <memory>

#include "Scene/StageConfigLoader.hpp"
#include "Scene/Stage4/Stage4.hpp"
#include "Scene/Stage3/Stage3Script.hpp"
#include "Scene/Title.hpp"

namespace {

const StageConfig& Stage3Config() {
    return LoadStageConfig("stage3");
}

}  // namespace

Stage3::Stage3(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, Stage3Config().playable,
                    std::make_unique<Stage3Script>()) {
    SetBackground(CreateStageBackground(m_Renderer, Stage3Config()));
}

std::unique_ptr<Scene> Stage3::NextScene() {
    if (auto cheatStage = CreateCheatStageScene()) return cheatStage;
    if (ShouldReturnToTitle() || WasGameOver() || !StageClearStarted()) {
        return std::make_unique<Title>();
    }
    return std::make_unique<Stage4>(m_Character, m_SpellCard, m_GameManager);
}
