#ifndef SCENE_STAGE2_STAGE2_HPP
#define SCENE_STAGE2_STAGE2_HPP

#include <memory>

#include "GameManager.hpp"
#include "Player.hpp"
#include "Scene/PlayableStage.hpp"

class Stage2 : public PlayableStage {
   public:
    Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int STAGE_TOTAL_FRAMES = 7600;

    void OnAfterGameplayFrame(const BossHudState& bossHud) override;
    int  BossSkipFrame() const override;

    bool                              m_FinalBossWasSeen = false;
};

#endif  // SCENE_STAGE2_STAGE2_HPP
