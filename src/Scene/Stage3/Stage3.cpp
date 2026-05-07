#include "Scene/Stage3/Stage3.hpp"

#include <memory>

#include "Scene/Stage3/Stage3Script.hpp"
#include "Scene/Title.hpp"

namespace {
constexpr int   STAGE3_FINAL_BOSS_FRAME = 5854;
}  // namespace

Stage3::Stage3(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager,
                    GA_RESOURCE_DIR "/stages/stage3_timeline.json",
                    std::make_unique<Stage3Script>(), "STAGE 3", "The Scarlet Border",
                    "BGM: Shanghai Teahouse ~ Chinese Tea") {
    SetBackground(std::make_unique<Stage3CourtyardBackground>(
        m_Renderer, GA_RESOURCE_DIR "/th06c/th06c_ST_output/stg3bg", -10.0f));
}

int Stage3::BossSkipFrame() const {
    return STAGE3_FINAL_BOSS_FRAME;
}

void Stage3::OnAfterGameplayFrame(const BossHudState& bossHud) {
    if (m_StageFrame >= STAGE3_FINAL_BOSS_FRAME && bossHud.visible) {
        m_FinalBossWasSeen = true;
    } else if (m_FinalBossWasSeen && !bossHud.visible) {
        m_Done = true;
    }

    if (!m_FinalBossWasSeen && m_StageFrame > STAGE_TOTAL_FRAMES) {
        m_Done = true;
    }
}

std::unique_ptr<Scene> Stage3::NextScene() {
    return std::make_unique<Title>();
}
