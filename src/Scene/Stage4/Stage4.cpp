#include "Scene/Stage4/Stage4.hpp"

#include <memory>

#include "Scene/StageConfigLoader.hpp"
#include "Scene/Stage4/Stage4Script.hpp"
#include "Scene/Stage5/Stage5.hpp"
#include "Scene/Title.hpp"

namespace {

const StageConfig& Stage4Config() {
    return LoadStageConfig("stage4");
}

}  // namespace

Stage4::Stage4(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, Stage4Config().playable,
                    std::make_unique<Stage4Script>()) {
    SetBackground(CreateStageBackground(m_Renderer, Stage4Config()));
}

std::unique_ptr<Scene> Stage4::NextScene() {
    if (ShouldReturnToTitle() || WasGameOver() || !StageClearStarted()) {
        return std::make_unique<Title>();
    }
    return std::make_unique<Stage5>(m_Character, m_SpellCard, m_GameManager);
}
