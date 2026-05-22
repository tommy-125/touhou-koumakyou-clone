#ifndef SCENE_STAGE4_STAGE4_SCRIPT_HPP
#define SCENE_STAGE4_STAGE4_SCRIPT_HPP

#include "Scene/PatternStageScript.hpp"

class Stage4Script : public PatternStageScript {
   public:
    Stage4Script();
    void Preload(Anm::Manager& anm) override;
};

#endif  // SCENE_STAGE4_STAGE4_SCRIPT_HPP
