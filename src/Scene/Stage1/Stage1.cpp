#include "Scene/Stage1/Stage1.hpp"

#include <memory>

#include "Scene/Stage1/Stage1Script.hpp"
#include "Scene/Timeline.hpp"
#include "Scene/Title.hpp"
#include "Util/Input.hpp"

// Transcribed from ecldata1.txt timeline0 (Normal difficulty)
// Format: {frame, subId, x, y, life, score, mirrored}
static const TimelineEntry STAGE1_TIMELINE[] = {
    // Wave 1: sub0 from left
    {128, 0, 60.0f, -32.0f, 8, 300, false},
    {144, 0, 68.0f, -32.0f, 32, 300, false},
    {160, 0, 76.0f, -32.0f, 32, 300, false},
    {176, 0, 84.0f, -32.0f, 32, 300, false},
    {192, 0, 92.0f, -32.0f, 32, 300, false},

    // Wave 1: sub0 from right (mirror)
    {256, 0, 324.0f, -32.0f, 32, 300, true},
    {272, 0, 316.0f, -32.0f, 32, 300, true},
    {288, 0, 308.0f, -32.0f, 32, 300, true},
    {304, 0, 300.0f, -32.0f, 32, 300, true},
    {320, 0, 292.0f, -32.0f, 32, 300, true},
    {336, 0, 284.0f, -32.0f, 32, 300, true},
    {352, 0, 276.0f, -32.0f, 32, 300, true},
    {368, 0, 268.0f, -32.0f, 32, 300, true},

    // Wave 2: sub1 left + right pairs
    {432, 1, 32.0f, -32.0f, 32, 300, false},
    {432, 1, 352.0f, -32.0f, 32, 300, true},
    {448, 1, 48.0f, -32.0f, 32, 300, false},
    {448, 1, 336.0f, -32.0f, 32, 300, true},
    {464, 1, 64.0f, -32.0f, 32, 300, false},
    {464, 1, 320.0f, -32.0f, 32, 300, true},
    {480, 1, 80.0f, -32.0f, 32, 300, false},
    {480, 1, 304.0f, -32.0f, 32, 300, true},
    {496, 1, 96.0f, -32.0f, 32, 300, false},
    {496, 1, 288.0f, -32.0f, 32, 300, true},
    {512, 1, 112.0f, -32.0f, 32, 300, false},
    {512, 1, 272.0f, -32.0f, 32, 300, true},
    {528, 1, 128.0f, -32.0f, 32, 300, false},
    {528, 1, 256.0f, -32.0f, 32, 300, true},
    {544, 1, 144.0f, -32.0f, 32, 300, false},
    {544, 1, 240.0f, -32.0f, 32, 300, true},
    {560, 1, 160.0f, -32.0f, 32, 300, false},
    {560, 1, 224.0f, -32.0f, 32, 300, true},
    {576, 1, 176.0f, -32.0f, 32, 300, false},
    {576, 1, 208.0f, -32.0f, 8, 300, true},

    // Wave 3: sub2/sub3 scattered (PowerItem)
    {640, 2, 32.0f, -32.0f, 80, 700, false},
    {700, 3, 256.0f, -32.0f, 80, 700, true},
    {750, 3, 128.0f, -32.0f, 80, 700, false},
    {800, 2, 352.0f, -32.0f, 80, 700, true},
    {850, 3, 24.0f, -32.0f, 80, 700, false},
    {890, 3, 304.0f, -32.0f, 80, 700, true},
    {930, 2, 144.0f, -32.0f, 80, 700, false},
    {960, 3, 344.0f, -32.0f, 80, 700, true},
    {990, 3, 32.0f, -32.0f, 80, 700, false},
    {1020, 2, 240.0f, -32.0f, 80, 700, true},
    {1040, 3, 24.0f, -32.0f, 80, 700, false},
    {1060, 3, 304.0f, -32.0f, 80, 700, true},
    {1080, 2, 144.0f, -32.0f, 80, 700, false},
    {1090, 3, 344.0f, -32.0f, 80, 700, true},
    {1100, 2, 32.0f, -32.0f, 80, 700, false},
    {1110, 2, 240.0f, -32.0f, 80, 700, true},

    // Wave 4: random spawns (enemy_create_random / enemy_create_mirror_random)
    {1220, 0, 0, -32.0f, 32, 300, true, true},
    {1230, 0, 0, -32.0f, 32, 300, false, true},
    {1240, 1, 0, -32.0f, 32, 300, true, true},
    {1250, 0, 0, -32.0f, 32, 300, false, true},
    {1260, 1, 0, -32.0f, 32, 300, true, true},
    {1270, 0, 0, -32.0f, 32, 300, false, true},
    {1280, 0, 0, -32.0f, 32, 300, true, true},
    {1290, 1, 0, -32.0f, 32, 300, false, true},
    {1300, 1, 0, -32.0f, 32, 300, true, true},
    {1310, 0, 0, -32.0f, 32, 300, false, true},
    {1320, 0, 0, -32.0f, 32, 300, false, true},
    {1330, 0, 0, -32.0f, 32, 300, true, true},
    {1340, 0, 0, -32.0f, 32, 300, false, true},
    {1350, 1, 0, -32.0f, 32, 300, true, true},
    {1360, 1, 0, -32.0f, 32, 300, false, true},
    {1370, 0, 0, -32.0f, 32, 300, true, true},
    {1380, 0, 0, -32.0f, 32, 300, false, true},
    {1390, 1, 0, -32.0f, 32, 300, true, true},
    {1400, 1, 0, -32.0f, 32, 300, false, true},

    // Wave 5: formation (right column, mirrored sub0)
    {1600, 0, 320.0f, -32.0f, 3, 300, true},
    {1600, 0, 352.0f, -32.0f, 3, 300, true},
    {1616, 0, 312.0f, -32.0f, 3, 300, true},
    {1616, 0, 344.0f, -32.0f, 3, 300, true},
    {1632, 0, 304.0f, -32.0f, 3, 300, true},
    {1632, 0, 336.0f, -32.0f, 3, 300, true},
    {1648, 0, 296.0f, -32.0f, 3, 300, true},
    {1648, 0, 328.0f, -32.0f, 3, 300, true},
    {1664, 0, 288.0f, -32.0f, 3, 300, true},
    {1664, 0, 320.0f, -32.0f, 3, 300, true},
    {1680, 0, 280.0f, -32.0f, 3, 300, true},
    {1680, 0, 312.0f, -32.0f, 3, 300, true},
    // Wave 5: formation (left column, normal sub0)
    {1696, 0, 64.0f, -32.0f, 3, 300, false},
    {1696, 0, 96.0f, -32.0f, 3, 300, false},
    {1712, 0, 72.0f, -32.0f, 3, 300, false},
    {1712, 0, 104.0f, -32.0f, 3, 300, false},
    {1728, 0, 80.0f, -32.0f, 3, 300, false},
    {1728, 0, 112.0f, -32.0f, 3, 300, false},
    {1744, 0, 88.0f, -32.0f, 3, 300, false},
    {1744, 0, 120.0f, -32.0f, 3, 300, false},
    {1760, 0, 96.0f, -32.0f, 3, 300, false},
    {1760, 0, 128.0f, -32.0f, 3, 300, false},
    {1776, 0, 104.0f, -32.0f, 3, 300, false},
    {1776, 0, 136.0f, -32.0f, 3, 300, false},
    {1792, 0, 112.0f, -32.0f, 3, 300, false},
    {1792, 0, 144.0f, -32.0f, 3, 300, false},
    {1808, 0, 120.0f, -32.0f, 3, 300, false},
    {1808, 0, 152.0f, -32.0f, 3, 300, false},

    // Mid-boss (sub8): Rumia pre-boss form
    {2008, 8, 0.0f, 0.0f, 6000, 100000, false},

    // Wave 6: sub2/sub3 scattered (RandomItem)
    {2408, 2, 32.0f, -32.0f, 80, 700, false, false, -1},
    {2438, 3, 256.0f, -32.0f, 80, 700, true, false, -1},
    {2468, 3, 128.0f, -32.0f, 80, 700, false, false, -1},
    {2498, 2, 352.0f, -32.0f, 80, 700, true, false, -1},
    {2528, 3, 24.0f, -32.0f, 80, 700, false, false, -1},
    {2558, 3, 304.0f, -32.0f, 80, 700, true, false, -1},
    {2588, 2, 144.0f, -32.0f, 80, 700, false, false, -1},
    {2618, 3, 344.0f, -32.0f, 80, 700, true, false, -1},
    {2648, 3, 32.0f, -32.0f, 80, 700, false, false, -1},
    {2678, 2, 240.0f, -32.0f, 80, 700, true, false, -1},
    {2708, 3, 24.0f, -32.0f, 80, 700, false, false, -1},
    {2738, 3, 304.0f, -32.0f, 80, 700, true, false, -1},
    {2768, 2, 144.0f, -32.0f, 80, 700, false, false, -1},
    {2798, 3, 344.0f, -32.0f, 80, 700, true, false, -1},
    {2828, 2, 32.0f, -32.0f, 80, 700, false, false, -1},
    {2858, 2, 240.0f, -32.0f, 80, 700, true, false, -1},
    {2888, 2, 32.0f, -32.0f, 80, 700, false, false, -1},
    {2918, 3, 256.0f, -32.0f, 80, 700, true, false, -1},
    {2948, 3, 128.0f, -32.0f, 80, 700, false, false, -1},
    {2978, 2, 352.0f, -32.0f, 80, 700, true, false, -1},
    {3008, 3, 24.0f, -32.0f, 80, 700, false, false, -1},
    {3038, 3, 304.0f, -32.0f, 80, 700, true, false, -1},
    {3068, 2, 144.0f, -32.0f, 80, 700, false, false, -1},
    {3098, 3, 344.0f, -32.0f, 80, 700, true, false, -1},
    {3128, 3, 32.0f, -32.0f, 80, 700, false, false, -1},
    {3158, 2, 240.0f, -32.0f, 80, 700, true, false, -1},
    {3188, 3, 24.0f, -32.0f, 80, 700, false, false, -1},
    {3218, 3, 304.0f, -32.0f, 80, 700, true, false, -1},
    {3248, 2, 144.0f, -32.0f, 80, 700, false, false, -1},
    {3278, 3, 344.0f, -32.0f, 80, 700, true, false, -1},
    {3308, 2, 32.0f, -32.0f, 80, 700, false, false, -1},
    {3338, 2, 240.0f, -32.0f, 80, 700, true, false, -1},
    {3368, 2, 32.0f, -32.0f, 80, 700, false, false, -1},
    {3398, 3, 256.0f, -32.0f, 80, 700, true, false, -1},
    {3428, 3, 128.0f, -32.0f, 80, 700, false, false, -1},
    {3458, 2, 352.0f, -32.0f, 80, 700, true, false, -1},
    {3488, 3, 24.0f, -32.0f, 80, 700, false, false, -1},
    {3518, 3, 304.0f, -32.0f, 80, 700, true, false, -1},
    {3548, 2, 144.0f, -32.0f, 80, 700, false, false, -1},
    {3578, 3, 344.0f, -32.0f, 80, 700, true, false, -1},
    {3608, 3, 32.0f, -32.0f, 80, 700, false, false, -1},
    {3638, 2, 240.0f, -32.0f, 80, 700, true, false, -1},
    {3668, 3, 24.0f, -32.0f, 80, 700, false, false, -1},
    {3698, 3, 304.0f, -32.0f, 80, 700, true, false, -1},
    {3728, 2, 144.0f, -32.0f, 80, 700, false, false, -1},
    {3758, 3, 344.0f, -32.0f, 80, 700, true, false, -1},
    {3788, 2, 32.0f, -32.0f, 80, 700, false, false, -1},
    {3818, 2, 240.0f, -32.0f, 80, 700, true, false, -1},
    {3848, 2, 32.0f, -32.0f, 80, 700, false, false, -1},
    {3878, 3, 256.0f, -32.0f, 80, 700, true, false, -1},
    {3908, 3, 128.0f, -32.0f, 80, 700, false, false, -1},
    {3938, 2, 352.0f, -32.0f, 80, 700, true, false, -1},
    {3968, 3, 24.0f, -32.0f, 80, 700, false, false, -1},
    {3998, 3, 304.0f, -32.0f, 80, 700, true, false, -1},
    {4028, 2, 144.0f, -32.0f, 80, 700, false, false, -1},
    {4058, 3, 344.0f, -32.0f, 80, 700, true, false, -1},
    {4088, 3, 32.0f, -32.0f, 80, 700, false, false, -1},
    {4118, 2, 240.0f, -32.0f, 80, 700, true, false, -1},
    {4148, 3, 24.0f, -32.0f, 80, 700, false, false, -1},
    {4178, 3, 304.0f, -32.0f, 80, 700, true, false, -1},
    {4208, 2, 144.0f, -32.0f, 80, 700, false, false, -1},
    {4238, 3, 344.0f, -32.0f, 80, 700, true, false, -1},
    {4268, 2, 32.0f, -32.0f, 80, 700, false, false, -1},
    {4298, 2, 240.0f, -32.0f, 80, 700, true, false, -1},

    // Wave 7: two-column formation (left side)
    {4498, 0, 32.0f, -32.0f, 3, 300, false},
    {4498, 0, 64.0f, -32.0f, 3, 300, false},
    {4514, 0, 40.0f, -32.0f, 3, 300, false},
    {4514, 0, 72.0f, -32.0f, 3, 300, false},
    {4530, 0, 48.0f, -32.0f, 3, 300, false},
    {4530, 0, 80.0f, -32.0f, 3, 300, false},
    {4546, 0, 56.0f, -32.0f, 3, 300, false},
    {4546, 0, 88.0f, -32.0f, 3, 300, false},
    {4562, 0, 64.0f, -32.0f, 3, 300, false},
    {4562, 0, 96.0f, -32.0f, 3, 300, false},
    {4578, 0, 72.0f, -32.0f, 3, 300, false},
    {4578, 0, 104.0f, -32.0f, 3, 300, false},
    {4594, 0, 80.0f, -32.0f, 3, 300, false},
    {4594, 0, 112.0f, -32.0f, 3, 300, false},
    // Wave 7: right column (mirrored)
    {4610, 0, 320.0f, -32.0f, 3, 300, true},
    {4610, 0, 352.0f, -32.0f, 3, 300, true},
    {4626, 0, 312.0f, -32.0f, 3, 300, true},
    {4626, 0, 344.0f, -32.0f, 3, 300, true},
    {4642, 0, 304.0f, -32.0f, 3, 300, true},
    {4642, 0, 336.0f, -32.0f, 3, 300, true},
    {4658, 0, 296.0f, -32.0f, 3, 300, true},
    {4658, 0, 328.0f, -32.0f, 3, 300, true},
    {4674, 0, 288.0f, -32.0f, 3, 300, true},
    {4674, 0, 320.0f, -32.0f, 3, 300, true},
    {4690, 0, 280.0f, -32.0f, 3, 300, true},
    {4690, 0, 312.0f, -32.0f, 3, 300, true},
    {4706, 0, 272.0f, -32.0f, 3, 300, true},
    {4706, 0, 304.0f, -32.0f, 3, 300, true},
    {4722, 0, 264.0f, -32.0f, 3, 300, true},
    {4722, 0, 296.0f, -32.0f, 3, 300, true},
    // Wave 7: left again
    {4738, 0, 32.0f, -32.0f, 3, 300, false},
    {4738, 0, 64.0f, -32.0f, 3, 300, false},
    {4754, 0, 40.0f, -32.0f, 3, 300, false},
    {4754, 0, 72.0f, -32.0f, 3, 300, false},
    {4770, 0, 48.0f, -32.0f, 3, 300, false},
    {4770, 0, 80.0f, -32.0f, 3, 300, false},
    {4786, 0, 56.0f, -32.0f, 3, 300, false},
    {4786, 0, 88.0f, -32.0f, 3, 300, false},
    {4802, 0, 64.0f, -32.0f, 3, 300, false},
    {4802, 0, 96.0f, -32.0f, 3, 300, false},
    {4818, 0, 72.0f, -32.0f, 3, 300, false},
    {4818, 0, 104.0f, -32.0f, 3, 300, false},
    {4834, 0, 80.0f, -32.0f, 3, 300, false},
    {4834, 0, 112.0f, -32.0f, 3, 300, false},
    {4850, 0, 88.0f, -32.0f, 3, 300, false},
    {4850, 0, 120.0f, -32.0f, 3, 300, false},
    // Wave 7: right again
    {4866, 0, 320.0f, -32.0f, 3, 300, true},
    {4866, 0, 352.0f, -32.0f, 3, 300, true},
    {4882, 0, 312.0f, -32.0f, 3, 300, true},
    {4882, 0, 344.0f, -32.0f, 3, 300, true},
    {4898, 0, 304.0f, -32.0f, 3, 300, true},
    {4898, 0, 336.0f, -32.0f, 3, 300, true},
    {4914, 0, 296.0f, -32.0f, 3, 300, true},
    {4914, 0, 328.0f, -32.0f, 3, 300, true},
    {4930, 0, 288.0f, -32.0f, 3, 300, true},
    {4930, 0, 320.0f, -32.0f, 3, 300, true},
    {4946, 0, 280.0f, -32.0f, 3, 300, true},
    {4946, 0, 312.0f, -32.0f, 3, 300, true},
    {4962, 0, 272.0f, -32.0f, 3, 300, true},
    {4962, 0, 304.0f, -32.0f, 3, 300, true},
    {4978, 0, 264.0f, -32.0f, 3, 300, true},
    {4978, 0, 296.0f, -32.0f, 3, 300, true},

    // Boss: Rumia
    {5279, 10, 0.0f, 0.0f, 1, 200000, false},
};

Stage1::Stage1(CharacterItem character, SpellCardItem spellCard) : m_Player(character, spellCard) {
    m_EnemyManager.SetItemManager(&m_ItemManager);
    m_EnemyManager.SetTimeline(STAGE1_TIMELINE,
                               sizeof(STAGE1_TIMELINE) / sizeof(STAGE1_TIMELINE[0]));
    m_EnemyManager.SetScript(std::make_unique<Stage1Script>());

    m_BgImage = std::make_shared<Util::Image>(GA_RESOURCE_DIR "/stage1_bg.png");
    m_BgObj   = std::make_shared<Util::GameObject>(m_BgImage, -10.0f);
    m_BgObj->m_Transform.translation = {-96.0f, 901.0f};
    m_Renderer.AddChild(m_BgObj);
}

void Stage1::Update() {
    if (Util::Input::IsKeyDown(Util::Keycode::F5)) {
        constexpr int BOSS_FRAME = 5200;
        m_StageFrame             = BOSS_FRAME;
        m_EnemyManager.SkipToFrame(BOSS_FRAME);
    }

    ++m_StageFrame;
    float scrollY = m_StageFrame * (BG_CANVAS_H - FIELD_H) / STAGE_TOTAL_FRAMES;
    m_BgObj->m_Transform.translation.y = (BG_CANVAS_H / 2.0f - FIELD_H / 2.0f) - scrollY;
    m_Renderer.Update();

    m_ItemManager.Update(m_Player.GetPos(), m_GameManager);
    m_EnemyManager.Update(m_Player.GetPos(), m_GameManager);
    m_Player.Update(m_GameManager);

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

    m_Gui.Update(m_GameManager);
}

std::unique_ptr<Scene> Stage1::NextScene() {
    return std::make_unique<Title>();
}
