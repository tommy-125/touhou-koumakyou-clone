#ifndef SCENE_STAGE2_STAGE2_HPP
#define SCENE_STAGE2_STAGE2_HPP

#include <array>
#include <memory>

#include "GameManager.hpp"
#include "Player.hpp"
#include "Scene/PlayableStage.hpp"
#include "Util/GameObject.hpp"
#include "Util/Image.hpp"

class Stage2 : public PlayableStage {
   public:
    Stage2(CharacterItem character, SpellCardItem spellCard, GameManager gameManager = {});
    std::unique_ptr<Scene> NextScene() override;

   private:
    static constexpr int STAGE_TOTAL_FRAMES = 7600;

    void UpdateBackground() override;
    void OnAfterGameplayFrame(const BossHudState& bossHud) override;
    int  BossSkipFrame() const override;

    std::shared_ptr<Util::Image>      m_BgImage;
    std::array<std::shared_ptr<Util::GameObject>, 3> m_BgObjs;
    bool                              m_FinalBossWasSeen = false;
};

#endif  // SCENE_STAGE2_STAGE2_HPP
