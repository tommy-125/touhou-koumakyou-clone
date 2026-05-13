#include "Scene/Stage6/Stage6.hpp"

#include <memory>

#include "Scene/Stage6/Stage6Script.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int STAGE6_FINAL_BOSS_FRAME = 3169;
static constexpr int STAGE6_TOTAL_FRAMES     = 16000;

PlayableStageConfig MakeStage6Config() {
    return {
        GA_RESOURCE_DIR "/stages/stage6_timeline.json",
        "STAGE 6",
        "The Young Descendant of Tepes",
        "BGM: The Young Descendant of Tepes",
        STAGE6_FINAL_BOSS_FRAME,
        STAGE6_TOTAL_FRAMES,
        6000,
        true,
        90,
    };
}

}  // namespace

Stage6::Stage6(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, MakeStage6Config(),
                    std::make_unique<Stage6Script>()) {
    SetBackground(std::make_unique<LongScrollStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/stage_backgrounds/stage6.png", -10.0f, -96.0f,
        BG_CANVAS_H, FIELD_H, STAGE_TOTAL_FRAMES));
}

std::unique_ptr<Scene> Stage6::NextScene() {
    return std::make_unique<Title>();
}
