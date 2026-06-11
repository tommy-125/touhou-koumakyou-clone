#include "Scene/Stage5/Stage5.hpp"

#include <memory>

#include "Scene/StageConfigLoader.hpp"
#include "Scene/Stage5/Stage5Script.hpp"
#include "Scene/Stage6/Stage6.hpp"
#include "Scene/Title.hpp"

namespace {

const StageConfig& Stage5Config() {
    return LoadStageConfig("stage5");
}

}  // namespace

Stage5::Stage5(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, Stage5Config().playable,
                    std::make_unique<Stage5Script>()) {
    SetBackground(CreateStageBackground(m_Renderer, Stage5Config()));
}

std::unique_ptr<Scene> Stage5::NextScene() {
    if (auto cheatStage = CreateCheatStageScene()) return cheatStage;
    if (ShouldReturnToTitle() || WasGameOver() || !StageClearStarted()) {
        return std::make_unique<Title>();
    }
    return std::make_unique<Stage6>(m_Character, m_SpellCard, m_GameManager);
}
