#ifndef SCENE_STAGE5_STAGE5SCRIPT_HPP
#define SCENE_STAGE5_STAGE5SCRIPT_HPP

#include "Scene/IStageScript.hpp"

class Stage5Script : public IStageScript {
   public:
    void Preload(Anm::Manager& anm) override;
    void InitSub(Enemy& enemy, EnemySubCtx& ctx) override;
    void RunSub(Enemy& enemy, EnemySubCtx& ctx) override;
};

#endif  // SCENE_STAGE5_STAGE5SCRIPT_HPP
