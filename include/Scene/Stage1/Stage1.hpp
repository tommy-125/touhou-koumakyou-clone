#ifndef SCENE_STAGE1_STAGE1_HPP
#define SCENE_STAGE1_STAGE1_HPP

#include <memory>

#include "Scene/PlayableStage.hpp"

class Stage1 : public PlayableStage {
   public:
    Stage1(CharacterItem character, SpellCardItem spellCard);
    std::unique_ptr<Scene> NextScene() override;
};

#endif  // SCENE_STAGE1_STAGE1_HPP
