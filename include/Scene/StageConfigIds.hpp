#ifndef SCENE_STAGE_CONFIG_IDS_HPP
#define SCENE_STAGE_CONFIG_IDS_HPP

#include <string_view>

namespace StageScriptUtil::ConfigId {
struct EnemyInitId {
    std::string_view value;
};

struct BossEntryId {
    std::string_view value;
};

struct BossPhaseId {
    std::string_view value;
};

struct MovementId {
    std::string_view value;
};

struct RewardId {
    std::string_view value;
};

namespace EnemyInit {
inline constexpr EnemyInitId Stage1SmallFairy{"stage1.small_fairy"};
inline constexpr EnemyInitId Stage1MediumFairy{"stage1.medium_fairy"};
inline constexpr EnemyInitId Stage2AngledFairy{"stage2.angled_fairy"};
inline constexpr EnemyInitId Stage2AimedFairy{"stage2.aimed_fairy"};
inline constexpr EnemyInitId Stage2RandomFairy{"stage2.random_fairy"};
inline constexpr EnemyInitId Stage2ShardFairy{"stage2.shard_fairy"};
inline constexpr EnemyInitId Stage2MediumFairy{"stage2.medium_fairy"};
inline constexpr EnemyInitId Stage3OpeningFairy{"stage3.opening_fairy"};
inline constexpr EnemyInitId Stage3SideFairy{"stage3.side_fairy"};
inline constexpr EnemyInitId Stage3Script15Fairy{"stage3.script15_fairy"};
inline constexpr EnemyInitId Stage3MediumRingFairy{"stage3.medium_ring_fairy"};
inline constexpr EnemyInitId Stage3BlueScatterFairy{"stage3.blue_scatter_fairy"};
inline constexpr EnemyInitId Stage4BookFairy{"stage4.book_fairy"};
inline constexpr EnemyInitId Stage4BurstFamiliar{"stage4.burst_familiar"};
inline constexpr EnemyInitId Stage4SmallFairy{"stage4.small_fairy"};
inline constexpr EnemyInitId Stage4RingCaster{"stage4.ring_caster"};
inline constexpr EnemyInitId Stage4DropProxy{"stage4.drop_proxy"};
inline constexpr EnemyInitId Stage4CrystalFairy{"stage4.crystal_fairy"};
inline constexpr EnemyInitId Stage5TopMaid{"stage5.top_maid"};
inline constexpr EnemyInitId Stage5SideMaid{"stage5.side_maid"};
inline constexpr EnemyInitId Stage5RingMaid{"stage5.ring_maid"};
inline constexpr EnemyInitId Stage5HelperProxy{"stage5.helper_proxy"};
inline constexpr EnemyInitId Stage6ArcFairyLeft{"stage6.arc_fairy_left"};
inline constexpr EnemyInitId Stage6ArcFairyRight{"stage6.arc_fairy_right"};
inline constexpr EnemyInitId Stage6BurstFairy{"stage6.burst_fairy"};
}  // namespace EnemyInit

namespace BossEntry {
inline constexpr BossEntryId Stage1RumiaMidboss{"stage1.rumia_midboss"};
inline constexpr BossEntryId Stage1RumiaBoss{"stage1.rumia_boss"};
inline constexpr BossEntryId Stage2Daiyousei{"stage2.daiyousei"};
inline constexpr BossEntryId Stage2Cirno{"stage2.cirno"};
inline constexpr BossEntryId Stage3MeilingMidboss{"stage3.meiling_midboss"};
inline constexpr BossEntryId Stage3MeilingBoss{"stage3.meiling_boss"};
inline constexpr BossEntryId Stage4KoakumaMidboss{"stage4.koakuma_midboss"};
inline constexpr BossEntryId Stage4Patchouli{"stage4.patchouli"};
inline constexpr BossEntryId Stage5Sakuya{"stage5.sakuya"};
inline constexpr BossEntryId Stage6Sakuya{"stage6.sakuya"};
inline constexpr BossEntryId Stage6Remilia{"stage6.remilia"};
}  // namespace BossEntry

namespace BossPhase {
inline constexpr BossPhaseId Stage1RumiaMidboss{"stage1.rumia_midboss"};
inline constexpr BossPhaseId Stage1RumiaFirstNonspell{"stage1.rumia_first_nonspell"};
inline constexpr BossPhaseId Stage1NightBird{"stage1.night_bird"};
inline constexpr BossPhaseId Stage1RumiaSecondNonspell{"stage1.rumia_second_nonspell"};
inline constexpr BossPhaseId Stage1Demarcation{"stage1.demarcation"};
inline constexpr BossPhaseId Stage2Daiyousei{"stage2.daiyousei"};
inline constexpr BossPhaseId Stage2CirnoFirstNonspell{"stage2.cirno_first_nonspell"};
inline constexpr BossPhaseId Stage2IcicleFall{"stage2.icicle_fall"};
inline constexpr BossPhaseId Stage2CirnoSecondNonspell{"stage2.cirno_second_nonspell"};
inline constexpr BossPhaseId Stage2PerfectFreeze{"stage2.perfect_freeze"};
inline constexpr BossPhaseId Stage2DiamondBlizzard{"stage2.diamond_blizzard"};
inline constexpr BossPhaseId Stage3GorgeousSweetFlower{"stage3.gorgeous_sweet_flower"};
inline constexpr BossPhaseId Stage3MeilingFirstNonspell{"stage3.meiling_first_nonspell"};
inline constexpr BossPhaseId Stage3RainbowWindChime{"stage3.rainbow_wind_chime"};
inline constexpr BossPhaseId Stage3MeilingSecondNonspell{"stage3.meiling_second_nonspell"};
inline constexpr BossPhaseId Stage3MeilingFinalNonspell{"stage3.meiling_final_nonspell"};
inline constexpr BossPhaseId Stage3ColorfulRain{"stage3.colorful_rain"};
inline constexpr BossPhaseId Stage3ExtremeColorTyphoon{"stage3.extreme_color_typhoon"};
inline constexpr BossPhaseId Stage4PatchouliFirstNonspell{"stage4.patchouli_first_nonspell"};
inline constexpr BossPhaseId Stage4PatchouliSecondNonspell{"stage4.patchouli_second_nonspell"};
inline constexpr BossPhaseId Stage4PrincessUndine{"stage4.princess_undine"};
inline constexpr BossPhaseId Stage4SylphyHornAdvanced{"stage4.sylphy_horn_advanced"};
inline constexpr BossPhaseId Stage4FinalNonspell{"stage4.final_nonspell"};
inline constexpr BossPhaseId Stage4PhaseOut{"stage4.phase_out"};
inline constexpr BossPhaseId Stage4WaterElf{"stage4.water_elf"};
inline constexpr BossPhaseId Stage5MidbossMain{"stage5.midboss_main"};
inline constexpr BossPhaseId Stage5Misdirection{"stage5.misdirection"};
inline constexpr BossPhaseId Stage5SakuyaFirstNonspell{"stage5.sakuya_first_nonspell"};
inline constexpr BossPhaseId Stage5SakuyaSecondNonspell{"stage5.sakuya_second_nonspell"};
inline constexpr BossPhaseId Stage5SakuyaFinalNonspell{"stage5.sakuya_final_nonspell"};
inline constexpr BossPhaseId Stage5ClockCorpse{"stage5.clock_corpse"};
inline constexpr BossPhaseId Stage5LunaClock{"stage5.luna_clock"};
inline constexpr BossPhaseId Stage5ManipulatingDoll{"stage5.manipulating_doll"};
inline constexpr BossPhaseId Stage6SakuyaMain{"stage6.sakuya_main"};
inline constexpr BossPhaseId Stage6EternalMeek{"stage6.eternal_meek"};
inline constexpr BossPhaseId Stage6RemiliaNonspell1{"stage6.remilia_nonspell_1"};
inline constexpr BossPhaseId Stage6RemiliaNonspell2{"stage6.remilia_nonspell_2"};
inline constexpr BossPhaseId Stage6RemiliaNonspell3{"stage6.remilia_nonspell_3"};
inline constexpr BossPhaseId Stage6RemiliaNonspell4{"stage6.remilia_nonspell_4"};
inline constexpr BossPhaseId Stage6StarOfDavid{"stage6.star_of_david"};
inline constexpr BossPhaseId Stage6ScarletNetherworld{"stage6.scarlet_netherworld"};
inline constexpr BossPhaseId Stage6VladTepes{"stage6.vlad_tepes"};
inline constexpr BossPhaseId Stage6ScarletShoot{"stage6.scarlet_shoot"};
inline constexpr BossPhaseId Stage6RedMagic{"stage6.red_magic"};
}  // namespace BossPhase

namespace Movement {
inline constexpr MovementId Stage1SmallFairyA{"stage1.small_fairy_a"};
inline constexpr MovementId Stage1SmallFairyB{"stage1.small_fairy_b"};
inline constexpr MovementId Stage1MediumFairy{"stage1.medium_fairy"};
inline constexpr MovementId Stage2AimedFairy{"stage2.aimed_fairy"};
inline constexpr MovementId Stage2MediumFairy{"stage2.medium_fairy"};
inline constexpr MovementId Stage3OpeningFairy{"stage3.opening_fairy"};
inline constexpr MovementId Stage3SideFairy{"stage3.side_fairy"};
inline constexpr MovementId Stage3Script15StopFairy{"stage3.script15_stop_fairy"};
inline constexpr MovementId Stage3WhiteRandomFairy{"stage3.white_random_fairy"};
inline constexpr MovementId Stage3BlueScatterFairy{"stage3.blue_scatter_fairy"};
inline constexpr MovementId Stage4SmallStraightSlow{"stage4.small_straight_slow"};
inline constexpr MovementId Stage4SmallStraightFast{"stage4.small_straight_fast"};
inline constexpr MovementId Stage4SmallStraightNoTurn{"stage4.small_straight_no_turn"};
inline constexpr MovementId Stage4SmallDiagonalSlow{"stage4.small_diagonal_slow"};
inline constexpr MovementId Stage4SmallDiagonalFast{"stage4.small_diagonal_fast"};
inline constexpr MovementId Stage4SmallDiagonalShort{"stage4.small_diagonal_short"};
inline constexpr MovementId Stage4SmallDown{"stage4.small_down"};
inline constexpr MovementId Stage4BookFairy{"stage4.book_fairy"};
inline constexpr MovementId Stage4RingCaster{"stage4.ring_caster"};
inline constexpr MovementId Stage5TopMaid{"stage5.top_maid"};
inline constexpr MovementId Stage5TopMaidDense{"stage5.top_maid_dense"};
inline constexpr MovementId Stage5SideMaid{"stage5.side_maid"};
inline constexpr MovementId Stage5RingMaid{"stage5.ring_maid"};
inline constexpr MovementId Stage6ArcFairyLow{"stage6.arc_fairy_low"};
inline constexpr MovementId Stage6ArcFairyHigh{"stage6.arc_fairy_high"};
inline constexpr MovementId Stage6BurstFairySide{"stage6.burst_fairy_side"};
inline constexpr MovementId Stage6BurstFairyTop{"stage6.burst_fairy_top"};
}  // namespace Movement

namespace Reward {
inline constexpr RewardId Power3CancelDie{"power_3_cancel_die"};
inline constexpr RewardId Power4Die{"power_4_die"};
inline constexpr RewardId Power5{"power_5"};
inline constexpr RewardId Power5Cancel{"power_5_cancel"};
inline constexpr RewardId Power5Die{"power_5_die"};
inline constexpr RewardId Power6Die{"power_6_die"};
inline constexpr RewardId Power8{"power_8"};
inline constexpr RewardId Power8Die{"power_8_die"};
inline constexpr RewardId Power8Bomb{"power_8_bomb"};
inline constexpr RewardId Power10LifeCancel{"power_10_life_cancel"};
inline constexpr RewardId Power12{"power_12"};
inline constexpr RewardId Power20BombCancelDie{"power_20_bomb_cancel_die"};
inline constexpr RewardId BombDie{"bomb_die"};
inline constexpr RewardId Life{"life"};
inline constexpr RewardId LifeToExit{"life_to_exit"};
}  // namespace Reward
}  // namespace StageScriptUtil::ConfigId

#endif  // SCENE_STAGE_CONFIG_IDS_HPP
