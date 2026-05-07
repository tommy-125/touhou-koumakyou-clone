#ifndef SCENE_STAGE2_STAGE2_SCRIPT_HPP
#define SCENE_STAGE2_STAGE2_SCRIPT_HPP

#include "Scene/IStageScript.hpp"

class Stage2Script : public IStageScript {
   public:
    void Preload(Anm::Manager& anm) override;
    void InitSub(Enemy& e, EnemySubCtx& ctx) override;
    void RunSub(Enemy& e, EnemySubCtx& ctx) override;
};

#endif  // SCENE_STAGE2_STAGE2_SCRIPT_HPP
