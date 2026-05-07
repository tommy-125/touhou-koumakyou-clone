#include "Scene/Stage2/Stage2.hpp"

#include <memory>

#include "Scene/Stage2/Stage2Script.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int        STAGE2_FINAL_BOSS_FRAME = 5984;
static constexpr float      BG_CENTER_X            = -96.0f;
static constexpr float      BG_SCALE               = 1.85f;
static constexpr float      BG_TILE_SIZE           = 256.0f;
static constexpr float      BG_SCROLL_SPEED        = 0.75f;

}  // namespace

Stage2::Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager,
                    GA_RESOURCE_DIR "/stages/stage2_timeline.json",
                    std::make_unique<Stage2Script>(), "STAGE 2",
                    "The Lake in the Moonlight", "BGM: Lunate Elf") {
    SetBackground(std::make_unique<TiledStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/th06c/th06c_ST/stg2bg.png", -10.0f, BG_CENTER_X,
        BG_SCALE, BG_TILE_SIZE, BG_SCROLL_SPEED, 2.0f, 0.01f));
}

int Stage2::BossSkipFrame() const {
    return STAGE2_FINAL_BOSS_FRAME;
}

void Stage2::OnAfterGameplayFrame(const BossHudState& bossHud) {
    if (m_StageFrame >= STAGE2_FINAL_BOSS_FRAME && bossHud.visible) {
        m_FinalBossWasSeen = true;
    } else if (m_FinalBossWasSeen && !bossHud.visible) {
        m_Done = true;
    }

    if (!m_FinalBossWasSeen && m_StageFrame > STAGE_TOTAL_FRAMES) {
        m_Done = true;
    }
}

std::unique_ptr<Scene> Stage2::NextScene() {
    return std::make_unique<Title>();
}
