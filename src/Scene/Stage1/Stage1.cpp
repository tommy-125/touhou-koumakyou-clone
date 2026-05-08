#include "Scene/Stage1/Stage1.hpp"

#include <memory>

#include "Scene/Stage1/Stage1Script.hpp"
#include "Scene/Stage2/Stage2.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int STAGE1_BOSS_SKIP_FRAME = 5279;
static constexpr int STAGE1_TOTAL_FRAMES    = 11600;

PlayableStageConfig MakeStage1Config() {
    return {
        GA_RESOURCE_DIR "/stages/stage1_timeline.json",
        "STAGE 1",
        "A Dream More Scarlet than Red",
        "BGM: A Soul as Red as a Ground Cherry",
        STAGE1_BOSS_SKIP_FRAME,
        STAGE1_TOTAL_FRAMES,
        1000,
        true,
        60,
    };
}

}  // namespace

Stage1::Stage1(CharacterItem character, SpellCardItem spellCard)
    : PlayableStage(character, spellCard, {}, MakeStage1Config(),
                    std::make_unique<Stage1Script>()) {
    SetBackground(std::make_unique<LongScrollStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/stage_backgrounds/stage1.png", -10.0f, -96.0f,
        BG_CANVAS_H, FIELD_H, STAGE_TOTAL_FRAMES));
}

std::unique_ptr<Scene> Stage1::NextScene() {
    if (ShouldReturnToTitle() || WasGameOver()) return std::make_unique<Title>();
    return std::make_unique<Stage2>(m_Character, m_SpellCard, m_GameManager);
}
