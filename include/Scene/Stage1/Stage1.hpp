#ifndef SCENE_STAGE1_STAGE1_HPP
#define SCENE_STAGE1_STAGE1_HPP

#include <memory>

#include "Scene/PlayableStage.hpp"

class Stage1 : public PlayableStage {
   public:
    Stage1(CharacterItem character, SpellCardItem spellCard);
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int   STAGE_TOTAL_FRAMES = 11600;
    static constexpr float BG_CANVAS_H        = 2250.0f;
    static constexpr float FIELD_H            = 448.0f;
};

#endif  // SCENE_STAGE1_STAGE1_HPP
