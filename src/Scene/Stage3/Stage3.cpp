#include "Scene/Stage3/Stage3.hpp"

#include <memory>

#include "Scene/Stage3/Stage3Script.hpp"
#include "Scene/Title.hpp"

namespace {
static constexpr int STAGE3_FINAL_BOSS_FRAME = 5854;
static constexpr int STAGE3_TOTAL_FRAMES     = 9000;

PlayableStageConfig MakeStage3Config() {
    return {
        GA_RESOURCE_DIR "/stages/stage3_timeline.json",
        "STAGE 3",
        "The Scarlet Border",
        "BGM: Shanghai Teahouse ~ Chinese Tea",
        STAGE3_FINAL_BOSS_FRAME,
        STAGE3_TOTAL_FRAMES,
        0,
        false,
        60,
    };
}

}  // namespace

Stage3::Stage3(CharacterItem character, SpellCardItem spellCard, GameManager gameManager)
    : PlayableStage(character, spellCard, gameManager, MakeStage3Config(),
                    std::make_unique<Stage3Script>()) {
    SetBackground(std::make_unique<LongScrollStageBackground>(
        m_Renderer, GA_RESOURCE_DIR "/stage_backgrounds/stage3.png", -10.0f, -96.0f,
        BG_CANVAS_H, FIELD_H, STAGE_TOTAL_FRAMES));
}

std::unique_ptr<Scene> Stage3::NextScene() {
    return std::make_unique<Title>();
}
