#ifndef SCENE_STAGE1_STAGE1_SCRIPT_HPP
#define SCENE_STAGE1_STAGE1_SCRIPT_HPP

#include "Scene/PatternStageScript.hpp"

class Stage1Script : public PatternStageScript {
   public:
    Stage1Script();
    void Preload(Anm::Manager& anm) override;
};

#endif  // SCENE_STAGE1_STAGE1_SCRIPT_HPP
