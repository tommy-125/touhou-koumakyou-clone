#ifndef SCENE_STAGE1_STAGE1_SCRIPT_HPP
#define SCENE_STAGE1_STAGE1_SCRIPT_HPP

#include "Scene/IStageScript.hpp"

class Stage1Script : public IStageScript {
   public:
    void Preload(Anm::Manager& anm) override;
    void InitSub(Enemy& e, EnemySubCtx& ctx) override;
    void RunSub(Enemy& e, EnemySubCtx& ctx) override;
};

#endif  // SCENE_STAGE1_STAGE1_SCRIPT_HPP
