#include "Scene/Stage5/Stage5.hpp"

#include <memory>

#include "Scene/Stage5/Stage5Script.hpp"
#include "Scene/Stage6/Stage6.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int STAGE5_MIDBOSS_FRAME   = 3372;
static constexpr int STAGE5_FINAL_BOSS_FRAME = 7704;
static constexpr int STAGE5_TOTAL_FRAMES     = 12500;

PlayableStageConfig MakeStage5Config() {
    return {
        GA_RESOURCE_DIR "/stages/stage5_timeline.json",
        "STAGE 5",
        "The Maid and the Pocket Watch of Blood",
        "BGM: The Maid and the Pocket Watch of Blood",
        STAGE5_FINAL_BOSS_FRAME,
        STAGE5_MIDBOSS_FRAME,
        STAGE5_TOTAL_FRAMES,
        5000,
        true,
        60,
    };
}

}  // namespace

Stage5::Stage5(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, MakeStage5Config(),
                    std::make_unique<Stage5Script>()) {
    SetBackground(std::make_unique<LongScrollStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/stage_backgrounds/stage5.png", -10.0f, -96.0f,
        BG_CANVAS_H, FIELD_H, STAGE_TOTAL_FRAMES));
}

std::unique_ptr<Scene> Stage5::NextScene() {
    if (ShouldReturnToTitle() || WasGameOver() || !StageClearStarted()) {
        return std::make_unique<Title>();
    }
    return std::make_unique<Stage6>(m_Character, m_SpellCard, m_GameManager);
}
