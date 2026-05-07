#include "Scene/PlayableStage.hpp"

#include <memory>

#include "Anm/AnmDefs.hpp"
#include "Scene/TimelineLoader.hpp"
#include "Util/Input.hpp"

namespace {
static constexpr float INTRO_CENTER_X    = -96.0f;
static constexpr float INTRO_STAGE_NO_SCALE   = 1.0f;
static constexpr float INTRO_STAGE_NAME_SCALE = 0.9f;
static constexpr float INTRO_SONG_SCALE       = 0.62f;
static const glm::vec2 INTRO_SONG_POS         = {64.0f, -204.0f};
static const Util::Color INTRO_STAGE_YELLOW = Util::Color::FromRGB(255, 255, 64);
static const Util::Color INTRO_LIGHT_CYAN   = Util::Color::FromRGB(224, 255, 255);

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
    m_IntroAnm.LoadAnm(Anm::ASCII.folder, Anm::ASCII.txt, Anm::ASCII.offset);

    m_EnemyManager.SetItemManager(&m_ItemManager);
    m_EnemyManager.SetTimeline(LoadTimelineFromJson(timelinePath));
    m_EnemyManager.SetScript(std::move(script));

    SetupIntroAsciiLine(m_IntroStageNoLine, stageNo, {INTRO_CENTER_X, 42.0f},
                        INTRO_STAGE_NO_SCALE, Util::AsciiTextAlign::Center,
                        INTRO_STAGE_YELLOW);
    SetupIntroAsciiLine(m_IntroStageNameLine, stageName, {INTRO_CENTER_X, 16.0f},
                        INTRO_STAGE_NAME_SCALE, Util::AsciiTextAlign::Center,
                        INTRO_LIGHT_CYAN);
    SetupIntroAsciiLine(m_IntroSongLine, songName, INTRO_SONG_POS, INTRO_SONG_SCALE,
                        Util::AsciiTextAlign::Right, INTRO_LIGHT_CYAN);
    UpdateStageIntro();
}

void PlayableStage::SetupIntroAsciiLine(Util::AsciiTextLine& line, const std::string& text,
                                        glm::vec2 pos, float scale, Util::AsciiTextAlign align,
                                        const Util::Color& color) {
    line.Configure(m_IntroRenderer, m_IntroAnm, 30.0f);
    line.SetText(text, pos, scale, align, color);
}

void PlayableStage::UpdateStageIntro() {
    const float alpha   = IntroAlpha(m_StageFrame);
    const bool  visible = alpha > 0.0f;

    m_IntroStageNoLine.SetVisible(visible);
    m_IntroStageNameLine.SetVisible(visible);
    m_IntroSongLine.SetVisible(visible);
    m_IntroStageNoLine.SetAlpha(alpha);
    m_IntroStageNameLine.SetAlpha(alpha);
    m_IntroSongLine.SetAlpha(alpha);
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
