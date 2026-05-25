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
};

#endif  // SCENE_STAGE5_STAGE5_HPP
