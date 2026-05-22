#ifndef SCENE_STAGE5_STAGE5SCRIPT_HPP
#define SCENE_STAGE5_STAGE5SCRIPT_HPP

#include "Scene/PatternStageScript.hpp"

class Stage5Script : public PatternStageScript {
   public:
    Stage5Script();
    void Preload(Anm::Manager& anm) override;
};

#endif  // SCENE_STAGE5_STAGE5SCRIPT_HPP
