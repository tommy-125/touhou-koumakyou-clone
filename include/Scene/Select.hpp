#ifndef SCENE_SELECT_HPP
#define SCENE_SELECT_HPP

#include <memory>
#include <vector>

#include "Anm/AnmManager.hpp"
#include "Anm/AnmTypes.hpp"
#include "Util/GameObject.hpp"
#include "Util/BlackMask.hpp"

#include "Scene/Scene.hpp"

// Difficulty select
constexpr int SELECT_INTERRUPT_ENTER_DIFFICULTY_SELECT = 6;
constexpr int SELECT_INTERRUPT_SELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT = 8;
constexpr int SELECT_INTERRUPT_UNSELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT = 18;
constexpr int SELECT_INTERRUPT_RETURN_TITLE = 4;
constexpr int SELECT_INTERRUPT_DIFFICULTY_TITLE_ENTER_CHARA_SELECT = 7;

// Character select
constexpr int SELECT_INTERRUPT_ENTER_CHARA_SELECT = 7;
constexpr int SELECT_INTERRUPT_CHARA_ENTER_FROM_RIGHT = 10;
constexpr int SELECT_INTERRUPT_CHARA_ENTER_FROM_LEFT = 9;
constexpr int SELECT_INTERRUPT_CHARA_LEAVE_FROM_RIGHT = 12;
constexpr int SELECT_INTERRUPT_CHARA_LEAVE_FROM_LEFT = 11;
constexpr int SELECT_INTERRUPT_SELECTED_CHARA_ITEM_ENTER_SPELLCARD_SELECT = 19;
constexpr int SELECT_INTERRUPT_CHARA_ITEM_RETURN_DIFFICULTY_SELECT = 6;
constexpr int SELECT_INTERRUPT_CHARA_TITLE_LEAVE_CHARA_SELECT = -1;

// Spell card select
constexpr int SELECT_INTERRUPT_SPELLCARD_ALL_ITEM_ENTER_SPELLCARD_SELECT = 13;
constexpr int SELECT_INTERRUPT_SPELLCARD_ALL_ITEM_LEAVE_SPELLCARD_SELECT = -1; // include title

enum class SelectEvent {
    EnterDifficultySelect,
    EnterCharaSelect,
    ReturnDifficultySelect,
    ReturnTitle,
    SwapCharaItemRight,
    SwapCharaItemLeft,
    EnterSpellCardSelect,
    ReturnCharaSelect,
};

enum class SelectState {
    Difficulty,
    Character,
    SpellCard,
};

enum class DifficultyItem {
    Easy,
    Normal,
    Hard,
    Lunatic,
};

enum class CharacterItem {
    Reimu,
    Marisa,
};

enum class ReimuSpellCardItem {
    SpellCard1,
    SpellCard2,
};

enum class MarisaSpellCardItem {
    SpellCard1,
    SpellCard2,
};

constexpr int SELECT_DIFFICULTY_COUNT = static_cast<int>(DifficultyItem::Lunatic) + 1;
constexpr int SELECT_CHARACTER_COUNT = static_cast<int>(CharacterItem::Marisa) + 1;
constexpr int SELECT_CHARACTER_PART_COUNT = 2; // each character has 2 parts in the ANM (upper and lower)
constexpr int SELECT_SPELLCARD_COUNT = static_cast<int>(ReimuSpellCardItem::SpellCard2) + 1;

class Select : public Scene {
public:
    Select();

    void Update() override;
    std::unique_ptr<Scene> NextScene() override;
    void HandleInterruptEvent(SelectEvent event);
private:
    SelectState m_CurrentState = SelectState::Difficulty;
    DifficultyItem m_SelectedDifficultyItem = DifficultyItem::Normal;
    CharacterItem m_SelectedCharacterItem = CharacterItem::Reimu;
    ReimuSpellCardItem m_SelectedReimuSpellCardItem = ReimuSpellCardItem::SpellCard1;
    MarisaSpellCardItem m_SelectedMarisaSpellCardItem = MarisaSpellCardItem::SpellCard1;
    
    Anm::Manager m_Anm;

    std::vector<Anm::Vm> m_Vms;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;

    Anm::Vm* m_DifficultyTitleVm;

    std::vector<Anm::Vm*> m_DifficultyItemVms;
    std::vector<std::shared_ptr<Util::GameObject>> m_DifficultyItemObjs;

    Anm::Vm* m_CharacterTitleVm;

    std::vector<std::vector<Anm::Vm*>> m_CharacterItemVms;

    Anm::Vm* m_SpellCardTitleVm;

    std::vector<std::vector<Anm::Vm*>> m_SpellCardItemVms;
    std::shared_ptr<Util::GameObject> m_BgObj;
    Util::BlackMask m_EnterSelectBlackMask;

    int m_SelectedDifficultyItemIdx = 1; // default to Normal
    int m_SelectedCharacterItemIdx = 0; // default to Reimu
    int m_SelectedSpellCardItemIdx = 0; // default to first spell card for each character

    bool m_Quitting = false;
    int m_QuitTimer = 0; // quit timer for animation
};

#endif
