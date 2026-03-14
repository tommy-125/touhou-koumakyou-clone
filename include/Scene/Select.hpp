#ifndef SCENE_SELECT_HPP
#define SCENE_SELECT_HPP

#include <memory>
#include <vector>

#include "Anm/AnmManager.hpp"
#include "Util/GameObject.hpp"
#include "Util/BlackMask.hpp"

#include "Scene/Scene.hpp"

constexpr int SELECT_INTERRUPT_ENTER_DIFFICULTY_SELECT = 6;
constexpr int SELECT_INTERRUPT_SELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT = 8;
constexpr int SELECT_INTERRUPT_UNSELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT = 18;
constexpr int SELECT_INTERRUPT_RETURN_TITLE = 4;

constexpr int SELECT_EVENT_ENTER_DIFFICULTY_SELECT= 100;
constexpr int SELECT_EVENT_ENTER_CHARA_SELECT= 101;
constexpr int SELECT_EVENT_RETURN_DIFFICULTY_SELECT= 102;
constexpr int SELECT_EVENT_RETURN_TITLE = 103;


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
constexpr int SELECT_SPELL_CARD_COUNT = static_cast<int>(ReimuSpellCardItem::SpellCard2) + 1;

class Select : public Scene {
public:
    Select();

    void Update() override;
    std::unique_ptr<Scene> NextScene() override;
    void HandleInterruptEvent(const int interruptEvent);
private:
    SelectState m_CurrentState = SelectState::Difficulty;
    DifficultyItem m_SelectedDifficultyItem = DifficultyItem::Normal;
    CharacterItem m_SelectedCharacterItem = CharacterItem::Reimu;
    ReimuSpellCardItem m_SelectedReimuSpellCardItem = ReimuSpellCardItem::SpellCard1;
    MarisaSpellCardItem m_SelectedMarisaSpellCardItem = MarisaSpellCardItem::SpellCard1;
    
    Anm::Manager m_Anm;

    std::vector<Anm::Vm> m_Vms;
    std::vector<std::shared_ptr<Util::GameObject>> m_Objs;

    std::vector<Anm::Vm*> m_DifficultyItemVms;
    std::vector<std::shared_ptr<Util::GameObject>> m_DifficultyItemObjs;
    
    Anm::Vm* m_DifficultyTitleVm;

    std::shared_ptr<Util::GameObject> m_BgObj;
    Util::BlackMask m_EnterSelectBlackMask;

    int m_SelectedDifficultyItemIdx = 1; // default to Normal
    int m_SelectedCharacterItemIdx = 0; // default to Reimu
    int m_SelectedSpellCardItemIdx = 0; // default to first spell card for each character

    bool m_Quitting = false;
    int m_QuitTimer = 0; // quit timer for animation
};

#endif
