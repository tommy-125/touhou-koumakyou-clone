#ifndef SCENE_STAGE6_STAGE6_HPP
#define SCENE_STAGE6_STAGE6_HPP

#include <memory>

#include "GameManager.hpp"
#include "Player.hpp"
#include "Scene/PlayableStage.hpp"

class Stage6 : public PlayableStage {
   public:
    Stage6(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    std::unique_ptr<Scene> NextScene() override;
};

#endif  // SCENE_STAGE6_STAGE6_HPP
