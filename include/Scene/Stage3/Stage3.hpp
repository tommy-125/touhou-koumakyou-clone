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
};

#endif  // SCENE_STAGE3_STAGE3_HPP
