#ifndef SCENE_STAGE1_HPP
#define SCENE_STAGE1_HPP

#include "Player.hpp"
#include "Scene/Scene.hpp"

class Stage1 : public Scene {
public:
    Stage1(CharacterItem character, SpellCardItem spellCard);
    void Update() override;

private:
    Player m_Player;
};

#endif // SCENE_STAGE1_HPP
