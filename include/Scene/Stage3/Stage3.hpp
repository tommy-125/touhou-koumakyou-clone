#ifndef SCENE_STAGE3_STAGE3_HPP
#define SCENE_STAGE3_STAGE3_HPP

#include <memory>

#include "GameManager.hpp"
#include "Player.hpp"
#include "Scene/PlayableStage.hpp"

class Stage3 : public PlayableStage {
   public:
    Stage3(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int   STAGE_TOTAL_FRAMES = 9000;
    static constexpr float BG_CANVAS_H        = 4096.0f;
    static constexpr float FIELD_H            = 448.0f;
};

#endif  // SCENE_STAGE3_STAGE3_HPP
