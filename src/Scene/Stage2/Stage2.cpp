#include "Scene/Stage2/Stage2.hpp"

#include <memory>

#include "Scene/Stage2/Stage2Script.hpp"
#include "Scene/Stage3/Stage3.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int   STAGE2_FINAL_BOSS_FRAME = 5984;
static constexpr int   STAGE2_TOTAL_FRAMES     = 7600;
static constexpr float BG_CENTER_X             = -96.0f;
static constexpr float BG_SCALE                = 1.85f;
static constexpr float BG_TILE_SIZE            = 256.0f;
static constexpr float BG_SCROLL_SPEED         = 0.75f;

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
    SetBackground(std::make_unique<TiledStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/th06c/th06c_ST/stg2bg.png", -10.0f, BG_CENTER_X,
        BG_SCALE, BG_TILE_SIZE, BG_SCROLL_SPEED, 2.0f, 0.01f));
}

std::unique_ptr<Scene> Stage2::NextScene() {
    if (ShouldReturnToTitle() || WasGameOver() || !StageClearStarted()) {
        return std::make_unique<Title>();
    }
    return std::make_unique<Stage3>(m_Character, m_SpellCard, m_GameManager);
}
