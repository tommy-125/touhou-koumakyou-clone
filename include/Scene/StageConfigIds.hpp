#pragma once

namespace StageScriptUtil::ConfigId {
namespace EnemyInit {
inline constexpr char Stage1SmallFairy[]       = "stage1.small_fairy";
inline constexpr char Stage1MediumFairy[]      = "stage1.medium_fairy";
inline constexpr char Stage2AngledFairy[]      = "stage2.angled_fairy";
inline constexpr char Stage2AimedFairy[]       = "stage2.aimed_fairy";
inline constexpr char Stage2RandomFairy[]      = "stage2.random_fairy";
inline constexpr char Stage2ShardFairy[]       = "stage2.shard_fairy";
inline constexpr char Stage2MediumFairy[]      = "stage2.medium_fairy";
inline constexpr char Stage3OpeningFairy[]     = "stage3.opening_fairy";
inline constexpr char Stage3SideFairy[]        = "stage3.side_fairy";
inline constexpr char Stage3Script15Fairy[]    = "stage3.script15_fairy";
inline constexpr char Stage3MediumRingFairy[]  = "stage3.medium_ring_fairy";
inline constexpr char Stage3BlueScatterFairy[] = "stage3.blue_scatter_fairy";
inline constexpr char Stage4BookFairy[]        = "stage4.book_fairy";
inline constexpr char Stage4BurstFamiliar[]    = "stage4.burst_familiar";
inline constexpr char Stage4SmallFairy[]       = "stage4.small_fairy";
inline constexpr char Stage4RingCaster[]       = "stage4.ring_caster";
inline constexpr char Stage4DropProxy[]        = "stage4.drop_proxy";
inline constexpr char Stage4CrystalFairy[]     = "stage4.crystal_fairy";
inline constexpr char Stage5TopMaid[]          = "stage5.top_maid";
inline constexpr char Stage5SideMaid[]         = "stage5.side_maid";
inline constexpr char Stage5RingMaid[]         = "stage5.ring_maid";
inline constexpr char Stage5HelperProxy[]      = "stage5.helper_proxy";
inline constexpr char Stage6ArcFairyLeft[]     = "stage6.arc_fairy_left";
inline constexpr char Stage6ArcFairyRight[]    = "stage6.arc_fairy_right";
inline constexpr char Stage6BurstFairy[]       = "stage6.burst_fairy";
}  // namespace EnemyInit

namespace BossEntry {
inline constexpr char Stage1RumiaMidboss[]   = "stage1.rumia_midboss";
inline constexpr char Stage1RumiaBoss[]      = "stage1.rumia_boss";
inline constexpr char Stage2Daiyousei[]      = "stage2.daiyousei";
inline constexpr char Stage2Cirno[]          = "stage2.cirno";
inline constexpr char Stage3MeilingMidboss[] = "stage3.meiling_midboss";
inline constexpr char Stage3MeilingBoss[]    = "stage3.meiling_boss";
inline constexpr char Stage4KoakumaMidboss[] = "stage4.koakuma_midboss";
inline constexpr char Stage4Patchouli[]      = "stage4.patchouli";
inline constexpr char Stage5Sakuya[]         = "stage5.sakuya";
inline constexpr char Stage6Sakuya[]         = "stage6.sakuya";
inline constexpr char Stage6Remilia[]        = "stage6.remilia";
}  // namespace BossEntry

namespace BossPhase {
inline constexpr char Stage1RumiaMidboss[]         = "stage1.rumia_midboss";
inline constexpr char Stage1RumiaFirstNonspell[]   = "stage1.rumia_first_nonspell";
inline constexpr char Stage1NightBird[]            = "stage1.night_bird";
inline constexpr char Stage1RumiaSecondNonspell[]  = "stage1.rumia_second_nonspell";
inline constexpr char Stage1Demarcation[]          = "stage1.demarcation";
inline constexpr char Stage2Daiyousei[]            = "stage2.daiyousei";
inline constexpr char Stage2CirnoFirstNonspell[]   = "stage2.cirno_first_nonspell";
inline constexpr char Stage2IcicleFall[]           = "stage2.icicle_fall";
inline constexpr char Stage2CirnoSecondNonspell[]  = "stage2.cirno_second_nonspell";
inline constexpr char Stage2PerfectFreeze[]        = "stage2.perfect_freeze";
inline constexpr char Stage2DiamondBlizzard[]      = "stage2.diamond_blizzard";
inline constexpr char Stage3MeilingFirstNonspell[] = "stage3.meiling_first_nonspell";
inline constexpr char Stage3RainbowWindChime[]     = "stage3.rainbow_wind_chime";
inline constexpr char Stage3MeilingSecondNonspell[] = "stage3.meiling_second_nonspell";
inline constexpr char Stage3MeilingFinalNonspell[] = "stage3.meiling_final_nonspell";
inline constexpr char Stage3ColorfulRain[]         = "stage3.colorful_rain";
inline constexpr char Stage4PatchouliFirstNonspell[] = "stage4.patchouli_first_nonspell";
inline constexpr char Stage4PatchouliSecondNonspell[] = "stage4.patchouli_second_nonspell";
inline constexpr char Stage4PrincessUndine[]       = "stage4.princess_undine";
inline constexpr char Stage4SylphyHornAdvanced[]   = "stage4.sylphy_horn_advanced";
inline constexpr char Stage4FinalNonspell[]        = "stage4.final_nonspell";
inline constexpr char Stage4PhaseOut[]             = "stage4.phase_out";
inline constexpr char Stage4WaterElf[]             = "stage4.water_elf";
inline constexpr char Stage5MidbossMain[]          = "stage5.midboss_main";
inline constexpr char Stage5Misdirection[]         = "stage5.misdirection";
inline constexpr char Stage5SakuyaFirstNonspell[]  = "stage5.sakuya_first_nonspell";
inline constexpr char Stage5SakuyaSecondNonspell[] = "stage5.sakuya_second_nonspell";
inline constexpr char Stage5SakuyaFinalNonspell[]  = "stage5.sakuya_final_nonspell";
inline constexpr char Stage5ClockCorpse[]          = "stage5.clock_corpse";
inline constexpr char Stage5LunaClock[]            = "stage5.luna_clock";
inline constexpr char Stage5ManipulatingDoll[]     = "stage5.manipulating_doll";
inline constexpr char Stage6SakuyaMain[]           = "stage6.sakuya_main";
inline constexpr char Stage6EternalMeek[]          = "stage6.eternal_meek";
inline constexpr char Stage6RemiliaNonspell1[]     = "stage6.remilia_nonspell_1";
inline constexpr char Stage6RemiliaNonspell2[]     = "stage6.remilia_nonspell_2";
inline constexpr char Stage6RemiliaNonspell3[]     = "stage6.remilia_nonspell_3";
inline constexpr char Stage6RemiliaNonspell4[]     = "stage6.remilia_nonspell_4";
inline constexpr char Stage6StarOfDavid[]          = "stage6.star_of_david";
inline constexpr char Stage6ScarletNetherworld[]   = "stage6.scarlet_netherworld";
inline constexpr char Stage6VladTepes[]            = "stage6.vlad_tepes";
inline constexpr char Stage6ScarletShoot[]         = "stage6.scarlet_shoot";
inline constexpr char Stage6RedMagic[]             = "stage6.red_magic";
}  // namespace BossPhase

namespace Movement {
inline constexpr char Stage5TopMaid[]        = "stage5.top_maid";
inline constexpr char Stage5TopMaidDense[]   = "stage5.top_maid_dense";
inline constexpr char Stage5SideMaid[]       = "stage5.side_maid";
inline constexpr char Stage5RingMaid[]       = "stage5.ring_maid";
inline constexpr char Stage6ArcFairyLow[]    = "stage6.arc_fairy_low";
inline constexpr char Stage6ArcFairyHigh[]   = "stage6.arc_fairy_high";
inline constexpr char Stage6BurstFairySide[] = "stage6.burst_fairy_side";
inline constexpr char Stage6BurstFairyTop[]  = "stage6.burst_fairy_top";
}  // namespace Movement

namespace Reward {
inline constexpr char Power3CancelDie[]      = "power_3_cancel_die";
inline constexpr char Power4Die[]            = "power_4_die";
inline constexpr char Power5[]               = "power_5";
inline constexpr char Power5Cancel[]         = "power_5_cancel";
inline constexpr char Power5Die[]            = "power_5_die";
inline constexpr char Power6Die[]            = "power_6_die";
inline constexpr char Power8[]               = "power_8";
inline constexpr char Power8Die[]            = "power_8_die";
inline constexpr char Power8Bomb[]           = "power_8_bomb";
inline constexpr char Power10LifeCancel[]    = "power_10_life_cancel";
inline constexpr char Power12[]              = "power_12";
inline constexpr char Power20BombCancelDie[] = "power_20_bomb_cancel_die";
inline constexpr char BombDie[]              = "bomb_die";
inline constexpr char Life[]                 = "life";
inline constexpr char LifeToExit[]           = "life_to_exit";
}  // namespace Reward
}  // namespace StageScriptUtil::ConfigId
