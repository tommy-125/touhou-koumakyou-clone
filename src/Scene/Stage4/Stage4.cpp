#include "Scene/Stage4/Stage4.hpp"

#include <memory>

#include "Scene/Stage4/Stage4Script.hpp"
#include "Scene/Stage5/Stage5.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int STAGE4_FINAL_BOSS_FRAME = 10695;
static constexpr int STAGE4_TOTAL_FRAMES     = 12200;

PlayableStageConfig MakeStage4Config() {
    return {
        GA_RESOURCE_DIR "/stages/stage4_timeline.json",
        "STAGE 4",
        "Voile, the Magic Library",
        "BGM: Voile, the Magic Library",
        STAGE4_FINAL_BOSS_FRAME,
        STAGE4_TOTAL_FRAMES,
        4000,
        true,
        60,
    };
}

}  // namespace

Stage4::Stage4(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, MakeStage4Config(),
                    std::make_unique<Stage4Script>()) {
    SetBackground(std::make_unique<LongScrollStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/stage_backgrounds/stage4.png", -10.0f, -96.0f,
        BG_CANVAS_H, FIELD_H, STAGE_TOTAL_FRAMES));
}

std::unique_ptr<Scene> Stage4::NextScene() {
    if (ShouldReturnToTitle() || WasGameOver() || !StageClearStarted()) {
        return std::make_unique<Title>();
    }
    return std::make_unique<Stage5>(m_Character, m_SpellCard, m_GameManager);
}
