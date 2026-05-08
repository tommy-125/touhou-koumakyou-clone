#ifndef SCENE_STAGE2_STAGE2_HPP
#define SCENE_STAGE2_STAGE2_HPP

#include <memory>

#include "GameManager.hpp"
#include "Player.hpp"
#include "Scene/PlayableStage.hpp"

class Stage2 : public PlayableStage {
   public:
    Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int   STAGE_TOTAL_FRAMES = 7600;
    static constexpr float BG_CANVAS_H        = 4096.0f;
    static constexpr float FIELD_H            = 448.0f;
};

#endif  // SCENE_STAGE2_STAGE2_HPP
