#include "Gui.hpp"

#include <algorithm>
#include <cstdio>
#include <string>

#include "Anm/AnmDefs.hpp"
#include "Util/Color.hpp"

static constexpr int        FONT_SIZE     = 12;
static constexpr const char FONT_PATH[]   = PTSD_FONT_PATH;
static constexpr float      DIGIT_SCALE   = 1.0f;
static constexpr float      DIGIT_ADVANCE = 14.0f;
static constexpr float      DIGIT_WIDTH   = 16.0f;
static constexpr float      DIGIT_HEIGHT  = 16.0f;
static const Util::Color    BOSS_COLOR    = Util::Color::FromRGB(255, 96, 96);
static const Util::Color    BOSS_HP_COLOR = Util::Color::FromRGB(255, 220, 220);
static const Util::Color    BOSS_TIME_COLOR = Util::Color::FromRGB(255, 255, 96);
static const Util::Color    SPELL_NAME_COLOR = Util::Color::FromRGB(220, 240, 255);

static void SetupTextWithColor(std::shared_ptr<Util::Text>&       text,
                               std::shared_ptr<Util::GameObject>& obj, const std::string& str,
                               const Util::Color& color, float x, float y) {
    text                         = std::make_shared<Util::Text>(FONT_PATH, FONT_SIZE, str, color);
    obj                          = std::make_shared<Util::GameObject>(text, 10.0f);
    obj->m_Transform.translation = {x, y};
}

static std::string BuildBossBar(int life, int minLife, int maxLife) {
    constexpr int kWidth = 44;
    const int     denom  = std::max(1, maxLife - minLife);
    const int     num    = std::clamp(life - minLife, 0, denom);
    const int     filled = (num * kWidth + denom / 2) / denom;
    return "[" + std::string(filled, '=') + std::string(kWidth - filled, ' ') + "]";
}

static std::string BuildBossBarFromRatio(float ratio) {
    constexpr int kWidth = 44;
    ratio                = std::clamp(ratio, 0.0f, 1.0f);
    const int filled     = static_cast<int>(ratio * static_cast<float>(kWidth) + 0.5f);
    return "[" + std::string(filled, '=') + std::string(kWidth - filled, ' ') + "]";
}

static std::string BuildTimerText(int secondsRemaining) {
    char buf[16];
    snprintf(buf, sizeof(buf), "TIME %02d", std::clamp(secondsRemaining, 0, 99));
    return buf;
}

static std::string BuildPhaseText(const BossHudState& bossHud) {
    if (!bossHud.showSpellName || bossHud.title.empty()) return " ";
    return bossHud.title;
}

template <size_t N>
static void SetupDigitField(std::array<std::shared_ptr<Util::GameObject>, N>& digits,
                            Util::Renderer& renderer, float startX, float y) {
    for (size_t i = 0; i < N; i++) {
        digits[i] = std::make_shared<Util::GameObject>(nullptr, 10.0f);
        digits[i]->m_Transform.translation = {
            startX + DIGIT_WIDTH * 0.5f + static_cast<float>(i) * DIGIT_ADVANCE,
            y - DIGIT_HEIGHT * 0.5f,
        };
        digits[i]->m_Transform.scale       = {DIGIT_SCALE, DIGIT_SCALE};
        digits[i]->SetVisible(false);
        renderer.AddChild(digits[i]);
    }
}

template <size_t N>
static void SetDigitField(std::array<std::shared_ptr<Util::GameObject>, N>& digits,
                          const Anm::Manager& anm, int asciiOffset, const std::string& value,
                          bool rightAlign = true) {
    const size_t textLen = std::min(value.size(), N);
    const size_t pad     = rightAlign ? (N - textLen) : 0;

    for (size_t i = 0; i < N; i++) {
        const auto& obj = digits[i];
        if (!obj) continue;

        if ((rightAlign && i < pad) || (!rightAlign && i >= textLen)) {
            obj->SetVisible(false);
            continue;
        }

        const size_t srcIdx = rightAlign ? (i - pad) : i;
        const char   ch     = value[srcIdx];
        if (ch < '0' || ch > '9') {
            obj->SetVisible(false);
            continue;
        }

        obj->SetDrawable(anm.sprites[asciiOffset + static_cast<int>(ch) - 0x15].image);
        obj->SetVisible(true);
    }
}

template <size_t N>
static void SetAsciiField(std::array<std::shared_ptr<Util::GameObject>, N>& digits,
                          const Anm::Manager& anm, int asciiOffset, const std::string& value,
                          bool rightAlign = true) {
    const size_t textLen = std::min(value.size(), N);
    const size_t pad     = rightAlign ? (N - textLen) : 0;

    for (size_t i = 0; i < N; i++) {
        const auto& obj = digits[i];
        if (!obj) continue;

        if ((rightAlign && i < pad) || (!rightAlign && i >= textLen)) {
            obj->SetVisible(false);
            continue;
        }

        const size_t srcIdx = rightAlign ? (i - pad) : i;
        const char   ch     = value[srcIdx];
        if (ch < 0x15) {
            obj->SetVisible(false);
            continue;
        }

        obj->SetDrawable(anm.sprites[asciiOffset + static_cast<int>(ch) - 0x15].image);
        obj->SetVisible(true);
    }
}

Gui::Gui() {
    m_BgImage =
        std::make_shared<Util::Image>(GA_RESOURCE_DIR "/th06c/th06c_CM_output/front/bg.png");
    m_BgObj                          = std::make_shared<Util::GameObject>(m_BgImage, 8.0f);
    m_BgObj->m_Transform.translation = {0.0f, 0.0f};
    m_Renderer.AddChild(m_BgObj);

    const int off = Anm::FRONT.offset;
    m_Anm.LoadAnm(Anm::FRONT.folder, Anm::FRONT.txt, off);
    m_Anm.LoadAnm(Anm::ASCII.folder, Anm::ASCII.txt, Anm::ASCII.offset);

    // Animated decorations: scripts 0–5
    for (int i = 0; i < NUM_DECO; i++) {
        m_Anm.SetScript(m_DecoVms[i], off + i, off);
        m_DecoVms[i].zIndex = 10.0f;
        if (m_DecoVms[i].obj) {
            m_DecoVms[i].obj->SetZIndex(10.0f);
            m_Renderer.AddChild(m_DecoVms[i].obj);
        }
    }

    // HUD label sprites: scripts 9–15
    for (int i = 0; i < NUM_LABEL; i++) {
        m_Anm.SetScript(m_LabelVms[i], off + 9 + i, off);
        m_LabelVms[i].zIndex = 10.0f;
        if (m_LabelVms[i].obj) {
            m_LabelVms[i].obj->SetZIndex(10.0f);
            m_Renderer.AddChild(m_LabelVms[i].obj);
        }
    }

    // Life icons: script 16, one per slot, positions set manually
    for (int i = 0; i < MAX_LIVES; i++) {
        m_Anm.SetScript(m_LifeVms[i], off + 16, off);
        m_LifeVms[i].pos    = {496.0f + i * 16.0f, 122.0f};
        m_LifeVms[i].zIndex = 10.0f;
        if (m_LifeVms[i].obj) {
            m_Anm.UpdateObjects(m_LifeVms[i]);
            m_LifeVms[i].obj->SetZIndex(10.0f);
            m_LifeVms[i].obj->SetVisible(false);
            m_Renderer.AddChild(m_LifeVms[i].obj);
        }
    }

    // Bomb icons: script 17
    for (int i = 0; i < MAX_BOMBS; i++) {
        m_Anm.SetScript(m_BombVms[i], off + 17, off);
        m_BombVms[i].pos    = {496.0f + i * 16.0f, 146.0f};
        m_BombVms[i].zIndex = 10.0f;
        if (m_BombVms[i].obj) {
            m_Anm.UpdateObjects(m_BombVms[i]);
            m_BombVms[i].obj->SetZIndex(10.0f);
            m_BombVms[i].obj->SetVisible(false);
            m_Renderer.AddChild(m_BombVms[i].obj);
        }
    }

    // Text value objects (PTSD coords = th06 - (320, -240))
    // HiScore: th06(496,58)  → PTSD(176,182)
    // Score:   th06(496,82)  → PTSD(176,158)
    // Power:   th06(496,186) → PTSD(176,54)
    // Graze:   th06(496,206) → PTSD(176,34)
    SetupDigitField(m_HiScoreDigits, m_Renderer, 176.0f, 182.0f);
    SetupDigitField(m_ScoreDigits, m_Renderer, 176.0f, 158.0f);
    SetupDigitField(m_PowerDigits, m_Renderer, 176.0f, 54.0f);
    SetupDigitField(m_GrazeDigits, m_Renderer, 176.0f, 34.0f);
    SetupDigitField(m_PointDigits, m_Renderer, 176.0f, 14.0f);

    SetupTextWithColor(m_BossLabelText, m_BossLabelObj, "BOSS", BOSS_COLOR, -288.0f, 218.0f);
    SetupTextWithColor(m_BossBarText, m_BossBarObj, BuildBossBar(1, 0, 1), BOSS_COLOR, -252.0f,
                       218.0f);
    SetupTextWithColor(m_BossHpText, m_BossHpObj, "0000/0000", BOSS_HP_COLOR, -288.0f, 200.0f);
    SetupTextWithColor(m_BossTimerText, m_BossTimerObj, "TIME 00", BOSS_TIME_COLOR, 132.0f, 218.0f);
    SetupTextWithColor(m_BossTitleText, m_BossTitleObj, " ", SPELL_NAME_COLOR, -160.0f, 196.0f);

    m_Renderer.AddChild(m_BossLabelObj);
    m_Renderer.AddChild(m_BossBarObj);
    m_Renderer.AddChild(m_BossHpObj);
    m_Renderer.AddChild(m_BossTimerObj);
    m_Renderer.AddChild(m_BossTitleObj);

    m_BossLabelObj->SetVisible(false);
    m_BossBarObj->SetVisible(false);
    m_BossHpObj->SetVisible(false);
    m_BossTimerObj->SetVisible(false);
    m_BossTitleObj->SetVisible(false);
}

void Gui::Update(const GameManager& gm, const BossHudState& bossHud, bool tick) {
    if (tick) {
        for (auto& vm : m_DecoVms) m_Anm.UpdateObjects(vm);
        for (auto& vm : m_LabelVms) m_Anm.UpdateObjects(vm);
    }

    if (m_LastLives != gm.livesRemaining) {
        m_LastLives = gm.livesRemaining;
        for (int i = 0; i < MAX_LIVES; i++)
            if (m_LifeVms[i].obj) m_LifeVms[i].obj->SetVisible(i < gm.livesRemaining);
    }

    if (m_LastBombs != gm.bombsRemaining) {
        m_LastBombs = gm.bombsRemaining;
        for (int i = 0; i < MAX_BOMBS; i++)
            if (m_BombVms[i].obj) m_BombVms[i].obj->SetVisible(i < gm.bombsRemaining);
    }

    if (m_LastScore != gm.score) {
        m_LastScore = gm.score;
        char buf[16];
        snprintf(buf, sizeof(buf), "%09d", gm.score);
        SetDigitField(m_ScoreDigits, m_Anm, Anm::ASCII.offset, buf);
    }

    if (m_LastHiScore != gm.highScore) {
        m_LastHiScore = gm.highScore;
        char buf[16];
        snprintf(buf, sizeof(buf), "%09d", gm.highScore);
        SetDigitField(m_HiScoreDigits, m_Anm, Anm::ASCII.offset, buf);
    }

    if (m_LastPower != gm.power) {
        m_LastPower = gm.power;
        if (gm.power >= 128) {
            SetAsciiField(m_PowerDigits, m_Anm, Anm::ASCII.offset, "MAX", false);
        } else {
            SetDigitField(m_PowerDigits, m_Anm, Anm::ASCII.offset, std::to_string(gm.power),
                          false);
        }
    }

    if (m_LastGraze != gm.graze) {
        m_LastGraze = gm.graze;
        SetDigitField(m_GrazeDigits, m_Anm, Anm::ASCII.offset, std::to_string(gm.graze), false);
    }

    if (m_LastPointItems != gm.pointItems) {
        m_LastPointItems = gm.pointItems;
        SetDigitField(m_PointDigits, m_Anm, Anm::ASCII.offset, std::to_string(gm.pointItems),
                      false);
    }

    const int bossShow = bossHud.visible ? 1 : 0;
    if (m_LastBossShow != bossShow) {
        m_LastBossShow = bossShow;
        if (!bossHud.visible) m_BossBarRatioDisplay = 0.0f;
    }

    if (tick) {
        if (bossHud.visible) {
            m_BossUiAnim = std::min(1.0f, m_BossUiAnim + 0.08f);
        } else {
            m_BossUiAnim = std::max(0.0f, m_BossUiAnim - 0.08f);
        }
    }

    const bool bossUiVisible = m_BossUiAnim > 0.0f;
    m_BossLabelObj->SetVisible(bossUiVisible);
    m_BossBarObj->SetVisible(bossUiVisible);
    m_BossHpObj->SetVisible(bossUiVisible);
    m_BossTimerObj->SetVisible(bossUiVisible && bossHud.secondsRemaining > 0);
    m_BossTitleObj->SetVisible(bossUiVisible && bossHud.showSpellName && !bossHud.title.empty());

    const float showOffset = (1.0f - m_BossUiAnim) * 36.0f;
    m_BossLabelObj->m_Transform.translation = {-288.0f - showOffset, 218.0f};
    m_BossBarObj->m_Transform.translation   = {-252.0f - showOffset, 218.0f};
    m_BossHpObj->m_Transform.translation    = {-288.0f - showOffset, 200.0f};
    m_BossTimerObj->m_Transform.translation = {132.0f + showOffset, 218.0f};
    m_BossTitleObj->m_Transform.translation = {-160.0f, 196.0f - showOffset * 0.35f};

    bool bossBarChanged = false;
    if (bossHud.visible) {
        const int   denom     = std::max(1, bossHud.maxLife - bossHud.minLife);
        const int   num       = std::clamp(bossHud.life - bossHud.minLife, 0, denom);
        const float targetBar = static_cast<float>(num) / static_cast<float>(denom);
        if (tick && targetBar > m_BossBarRatioDisplay) {
            m_BossBarRatioDisplay += 0.01f;
            if (m_BossBarRatioDisplay > targetBar) m_BossBarRatioDisplay = targetBar;
            bossBarChanged = true;
        } else if (tick && targetBar < m_BossBarRatioDisplay) {
            m_BossBarRatioDisplay -= 0.02f;
            if (m_BossBarRatioDisplay < targetBar) m_BossBarRatioDisplay = targetBar;
            bossBarChanged = true;
        } else if (!tick && m_LastBossShow == 1) {
            m_BossBarRatioDisplay = targetBar;
        }
    }

    if (bossHud.visible && (m_LastBossLife != bossHud.life || m_LastBossMin != bossHud.minLife ||
                            m_LastBossMax != bossHud.maxLife || bossBarChanged)) {
        m_LastBossLife = bossHud.life;
        m_LastBossMin  = bossHud.minLife;
        m_LastBossMax  = bossHud.maxLife;

        m_BossBarText->SetText(BuildBossBarFromRatio(m_BossBarRatioDisplay));

        char hpBuf[32];
        snprintf(hpBuf, sizeof(hpBuf), "%d/%d", std::max(0, bossHud.life),
                 std::max(1, bossHud.maxLife));
        m_BossHpText->SetText(hpBuf);
    }

    if (m_LastBossSeconds != bossHud.secondsRemaining) {
        m_LastBossSeconds = bossHud.secondsRemaining;
        m_BossTimerText->SetText(BuildTimerText(bossHud.secondsRemaining));
    }

    if (m_LastBossTitle != bossHud.title || m_LastBossShowName != bossHud.showSpellName) {
        m_LastBossTitle    = bossHud.title;
        m_LastBossShowName = bossHud.showSpellName;
        m_BossTitleText->SetText(BuildPhaseText(bossHud));
    }

    m_Renderer.Update();
}
