#ifndef SCENE_STAGE6_STAGE6SCRIPT_HPP
#define SCENE_STAGE6_STAGE6SCRIPT_HPP

#include "Scene/PatternStageScript.hpp"

class Stage6Script : public PatternStageScript {
   public:
    Stage6Script();
    void Preload(Anm::Manager& anm) override;
};

#endif  // SCENE_STAGE6_STAGE6SCRIPT_HPP
