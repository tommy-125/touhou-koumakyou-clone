#ifndef SCENE_STAGE1_HPP
#define SCENE_STAGE1_HPP

#include "Character.hpp"
#include "Scene/Scene.hpp"

class Stage1 : public Scene {
public:
    Stage1(CharacterItem character, int spellCardIdx);
    void Update() override;

private:
    Character m_Character;
};

#endif // SCENE_STAGE1_HPP
