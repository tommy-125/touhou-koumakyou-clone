#ifndef SCENE_STAGE3_STAGE3_SCRIPT_HPP
#define SCENE_STAGE3_STAGE3_SCRIPT_HPP

#include "Scene/PatternStageScript.hpp"

class Stage3Script : public PatternStageScript {
   public:
    Stage3Script();
    void Preload(Anm::Manager& anm) override;
};

#endif  // SCENE_STAGE3_STAGE3_SCRIPT_HPP
