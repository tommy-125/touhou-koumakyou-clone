#include "Scene/PlayableStage.hpp"

#include <memory>

#include "Scene/TimelineLoader.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr int        INTRO_FONT_SIZE   = 18;
static constexpr int        INTRO_SMALL_SIZE  = 14;
static constexpr const char INTRO_FONT_PATH[] = PTSD_FONT_PATH;
static constexpr float      INTRO_CENTER_X    = -96.0f;
static const Util::Color    INTRO_CYAN        = Util::Color::FromRGB(190, 245, 255);
static const Util::Color    INTRO_YELLOW      = Util::Color::FromRGB(255, 230, 120);

static float IntroAlpha(int frame) {
    if (frame < 30) return static_cast<float>(frame) / 30.0f;
    if (frame < 210) return 1.0f;
    if (frame < 270) return 1.0f - static_cast<float>(frame - 210) / 60.0f;
    return 0.0f;
}

}  // namespace

PlayableStage::PlayableStage(CharacterItem character, SpellCardItem spellCard,
                             GameManager gameManager, const char* timelinePath,
                             std::unique_ptr<IStageScript> script, const std::string& stageNo,
                             const std::string& stageName, const std::string& songName)
    : m_Character(character),
      m_SpellCard(spellCard),
      m_GameManager(gameManager),
      m_Player(character, spellCard),
      m_StageMenu(m_Renderer) {
    m_EnemyManager.SetItemManager(&m_ItemManager);
    m_EnemyManager.SetTimeline(LoadTimelineFromJson(timelinePath));
    m_EnemyManager.SetScript(std::move(script));

    m_IntroStageNoObj = MakeIntroText(m_IntroStageNoText, stageNo, INTRO_YELLOW,
                                      INTRO_FONT_SIZE, INTRO_CENTER_X, 42.0f);
    m_IntroStageNameObj = MakeIntroText(m_IntroStageNameText, stageName, INTRO_CYAN,
                                        INTRO_FONT_SIZE, INTRO_CENTER_X, 16.0f);
    m_IntroSongObj = MakeIntroText(m_IntroSongText, songName, INTRO_CYAN, INTRO_SMALL_SIZE,
                                   INTRO_CENTER_X, -12.0f);
    m_IntroRenderer.AddChild(m_IntroStageNoObj);
    m_IntroRenderer.AddChild(m_IntroStageNameObj);
    m_IntroRenderer.AddChild(m_IntroSongObj);
    UpdateStageIntro();
}

std::shared_ptr<Util::GameObject> PlayableStage::MakeIntroText(
    std::shared_ptr<Util::Text>& text, const std::string& str, const Util::Color& color,
    int fontSize, float x, float y) {
    text = std::make_shared<Util::Text>(INTRO_FONT_PATH, fontSize, str, color);
    auto obj = std::make_shared<Util::GameObject>(text, 30.0f);
    obj->m_Transform.translation = {x, y};
    obj->SetVisible(false);
    return obj;
}

void PlayableStage::UpdateStageIntro() {
    const float alpha   = IntroAlpha(m_StageFrame);
    const bool  visible = alpha > 0.0f;
    const auto  alphaByte = static_cast<Uint8>(alpha * 255.0f);

    m_IntroStageNoObj->SetVisible(visible);
    m_IntroStageNameObj->SetVisible(visible);
    m_IntroSongObj->SetVisible(visible);
    if (!visible) return;

    m_IntroStageNoText->SetColor(Util::Color::FromRGB(255, 230, 120, alphaByte));
    m_IntroStageNameText->SetColor(Util::Color::FromRGB(190, 245, 255, alphaByte));
    m_IntroSongText->SetColor(Util::Color::FromRGB(190, 245, 255, alphaByte));
    m_IntroStageNoObj->SetAlpha(1.0f);
    m_IntroStageNameObj->SetAlpha(1.0f);
    m_IntroSongObj->SetAlpha(1.0f);
}

void PlayableStage::SetBackground(std::unique_ptr<StageBackground> background) {
    m_Background = std::move(background);
}

void PlayableStage::UpdateBackground() {
    if (m_Background) m_Background->Update(m_StageFrame);
}

void PlayableStage::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::ESCAPE)) {
        m_StageMenu.Toggle();
    }

    const auto stageMenuAction = m_StageMenu.Update();
    if (stageMenuAction == StageMenu::Action::ReturnToTitle) {
        m_ReturnToTitle = true;
        m_Done          = true;
        return;
    }

    const int bossSkipFrame = BossSkipFrame();
    if (!m_StageMenu.IsOpen() && bossSkipFrame >= 0 &&
        Util::Input::IsKeyDown(Util::Keycode::P)) {
        m_StageFrame = bossSkipFrame;
        m_EnemyManager.SkipToFrame(bossSkipFrame);
    }

    if (m_StageMenu.IsOpen()) {
        UpdateBackground();
        m_Renderer.Update();
        m_Gui.Update(m_GameManager, m_EnemyManager.GetBossHudState(), false);
        UpdateStageIntro();
        m_IntroRenderer.Update();
        OnMenuFrame();
        return;
    }

    if (HandleStageOverlay()) return;

    ++m_StageFrame;
    UpdateBackground();
    m_Renderer.Update();

    m_ItemManager.Update(m_Player.GetPos(), m_GameManager);
    m_EnemyManager.Update(m_Player.GetPos(), m_GameManager);
    m_Player.Update(m_GameManager);
    if (m_Player.TryUseBomb(m_GameManager) || m_Player.IsBombActive()) {
        m_EnemyManager.ClearAllBullets();
    }

    int scoreGained = m_EnemyManager.ApplyPlayerBulletDamage(m_Player);
    if (scoreGained > 0) {
        m_GameManager.score += scoreGained;
        if (m_GameManager.score > m_GameManager.highScore)
            m_GameManager.highScore = m_GameManager.score;
    }

    if (m_Player.IsVulnerable() &&
        m_EnemyManager.CheckPlayerHit(m_Player.GetPos(), {PLAYER_HITBOX_X, PLAYER_HITBOX_Y})) {
        m_Player.Die();
        if (--m_GameManager.livesRemaining < 0) {
            m_GameManager.livesRemaining = 0;
            m_Done                       = true;
        }
    }

    if (m_Player.JustEnteredSpawning()) {
        m_EnemyManager.ClearAllBullets();
    }

    const BossHudState bossHud = m_EnemyManager.GetBossHudState();
    OnAfterGameplayFrame(bossHud);

    m_Gui.Update(m_GameManager, bossHud, true);
    UpdateStageIntro();
    m_IntroRenderer.Update();
    OnFrameEnd();
}
