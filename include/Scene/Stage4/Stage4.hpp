#ifndef SCENE_STAGE4_STAGE4_HPP
#define SCENE_STAGE4_STAGE4_HPP

#include <memory>

#include "GameManager.hpp"
#include "Player.hpp"
#include "Scene/PlayableStage.hpp"

class Stage4 : public PlayableStage {
   public:
    Stage4(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    std::unique_ptr<Scene> NextScene() override;
};

#endif  // SCENE_STAGE4_STAGE4_HPP
