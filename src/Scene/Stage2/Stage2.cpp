#include "Scene/Stage2/Stage2.hpp"

#include <memory>

#include "Scene/Stage2/Stage2Script.hpp"
#include "Scene/Stage3/Stage3.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int   STAGE2_FINAL_BOSS_FRAME = 5984;
static constexpr int   STAGE2_TOTAL_FRAMES     = 7600;

PlayableStageConfig MakeStage2Config() {
    return {
        GA_RESOURCE_DIR "/stages/stage2_timeline.json",
        "STAGE 2",
        "The Lake in the Moonlight",
        "BGM: Lunate Elf",
        STAGE2_FINAL_BOSS_FRAME,
        STAGE2_TOTAL_FRAMES,
        2000,
        true,
        60,
    };
}

}  // namespace

Stage2::Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, MakeStage2Config(),
                    std::make_unique<Stage2Script>()) {
    SetBackground(std::make_unique<LongScrollStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/stage_backgrounds/stage2.png", -10.0f, -96.0f,
        BG_CANVAS_H, FIELD_H, STAGE_TOTAL_FRAMES));
}

std::unique_ptr<Scene> Stage2::NextScene() {
    if (ShouldReturnToTitle() || WasGameOver() || !StageClearStarted()) {
        return std::make_unique<Title>();
    }
    return std::make_unique<Stage3>(m_Character, m_SpellCard, m_GameManager);
}
