#include "Scene/Stage2/Stage2.hpp"

#include <cmath>
#include <memory>

#include "Scene/Stage2/Stage2Script.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int        STAGE2_FINAL_BOSS_FRAME = 5984;
static constexpr float      BG_CENTER_X            = -96.0f;
static constexpr float      BG_SCALE               = 1.85f;
static constexpr float      BG_TILE_H              = 256.0f * BG_SCALE;
static constexpr float      BG_SCROLL_SPEED        = 0.75f;

}  // namespace

Stage2::Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager,
                    GA_RESOURCE_DIR "/stages/stage2_timeline.json",
                    std::make_unique<Stage2Script>(), "STAGE 2",
                    "The Lake in the Moonlight", "BGM: Lunate Elf") {
    m_BgImage = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/th06c/th06c_ST/stg2bg.png");
    for (auto& bgObj : m_BgObjs) {
        bgObj                    = std::make_shared<Util::GameObject>(m_BgImage, -10.0f);
        bgObj->m_Transform.scale = {BG_SCALE, BG_SCALE};
        m_Renderer.AddChild(bgObj);
    }
    UpdateBackground();
}

void Stage2::UpdateBackground() {
    const float scroll = std::fmod(static_cast<float>(m_StageFrame) * BG_SCROLL_SPEED, BG_TILE_H);
    const float sway   = std::sin(static_cast<float>(m_StageFrame) * 0.01f) * 2.0f;

    for (size_t i = 0; i < m_BgObjs.size(); i++) {
        auto& bgObj = m_BgObjs[i];
        if (!bgObj) continue;
        const float tileOffset = (static_cast<float>(i) - 1.0f) * BG_TILE_H;
        bgObj->m_Transform.translation = {BG_CENTER_X + sway, tileOffset - scroll};
    }
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
