#ifndef SCENE_STAGE5_STAGE5_HPP
#define SCENE_STAGE5_STAGE5_HPP

#include <memory>

#include "GameManager.hpp"
#include "Player.hpp"
#include "Scene/PlayableStage.hpp"

class Stage5 : public PlayableStage {
   public:
    Stage5(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int   STAGE_TOTAL_FRAMES = 12500;
    static constexpr float BG_CANVAS_H        = 4096.0f;
    static constexpr float FIELD_H            = 448.0f;
};

#endif  // SCENE_STAGE5_STAGE5_HPP
