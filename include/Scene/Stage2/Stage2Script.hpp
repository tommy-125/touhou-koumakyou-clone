#ifndef SCENE_STAGE2_STAGE2_SCRIPT_HPP
#define SCENE_STAGE2_STAGE2_SCRIPT_HPP

#include "Scene/PatternStageScript.hpp"

class Stage2Script : public PatternStageScript {
   public:
    Stage2Script();
    void Preload(Anm::Manager& anm) override;
};

#endif  // SCENE_STAGE2_STAGE2_SCRIPT_HPP
