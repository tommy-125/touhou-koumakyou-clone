#ifndef SCENE_STAGE4_STAGE4_SCRIPT_HPP
#define SCENE_STAGE4_STAGE4_SCRIPT_HPP

#include "Scene/IStageScript.hpp"

class Stage4Script : public IStageScript {
   public:
    void Preload(Anm::Manager& anm) override;
    void InitSub(Enemy& e, EnemySubCtx& ctx) override;
    void RunSub(Enemy& e, EnemySubCtx& ctx) override;
};

#endif  // SCENE_STAGE4_STAGE4_SCRIPT_HPP
