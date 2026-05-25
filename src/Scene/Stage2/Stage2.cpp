#include "Scene/Stage2/Stage2.hpp"

#include <memory>

#include "Scene/StageConfigLoader.hpp"
#include "Scene/Stage2/Stage2Script.hpp"
#include "Scene/Stage3/Stage3.hpp"
#include "Scene/Title.hpp"

namespace {

const StageConfig& Stage2Config() {
    return LoadStageConfig("stage2");
}

}  // namespace

Stage2::Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, Stage2Config().playable,
                    std::make_unique<Stage2Script>()) {
    SetBackground(CreateStageBackground(m_Renderer, Stage2Config()));
}

std::unique_ptr<Scene> Stage2::NextScene() {
    if (ShouldReturnToTitle() || WasGameOver() || !StageClearStarted()) {
        return std::make_unique<Title>();
    }
    return std::make_unique<Stage3>(m_Character, m_SpellCard, m_GameManager);
}
