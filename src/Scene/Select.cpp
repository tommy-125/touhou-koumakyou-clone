#include "Scene/Select.hpp"

#include <memory>

#include "Anm/AnmDefs.hpp"
#include "Anm/AnmManager.hpp"
#include "Scene/Stage1/Stage1.hpp"
#include "Scene/Title.hpp"
#include "Util/Image.hpp"
#include "Util/Input.hpp"

Select::Select() : m_EnterSelectBlackMask(2.0f, 1.0f) {
    // Background
    auto bgImage =
        std::make_shared<Util::Image>(GA_RESOURCE_DIR "/th06c/th06c_TL_output/no_anm/select00.jpg");
    m_BgObj = std::make_shared<Util::GameObject>(bgImage, 0.0f);
    m_Renderer.AddChild(m_BgObj);

    // Load ANM
    std::vector<Anm::LoadedEntry> loaded = {
        {&Anm::SELECT01,
         m_Anm.LoadAnm(Anm::SELECT01.folder, Anm::SELECT01.txt, Anm::SELECT01.offset)},
        {&Anm::SELECT02,
         m_Anm.LoadAnm(Anm::SELECT02.folder, Anm::SELECT02.txt, Anm::SELECT02.offset)},
        {&Anm::SELECT03,
         m_Anm.LoadAnm(Anm::SELECT03.folder, Anm::SELECT03.txt, Anm::SELECT03.offset)},
        {&Anm::SELECT04,
         m_Anm.LoadAnm(Anm::SELECT04.folder, Anm::SELECT04.txt, Anm::SELECT04.offset)},
        {&Anm::SELECT05,
         m_Anm.LoadAnm(Anm::SELECT05.folder, Anm::SELECT05.txt, Anm::SELECT05.offset)},
        {&Anm::SLPL00A, m_Anm.LoadAnm(Anm::SLPL00A.folder, Anm::SLPL00A.txt, Anm::SLPL00A.offset)},
        {&Anm::SLPL00B, m_Anm.LoadAnm(Anm::SLPL00B.folder, Anm::SLPL00B.txt, Anm::SLPL00B.offset)},
        {&Anm::SLPL01A, m_Anm.LoadAnm(Anm::SLPL01A.folder, Anm::SLPL01A.txt, Anm::SLPL01A.offset)},
        {&Anm::SLPL01B, m_Anm.LoadAnm(Anm::SLPL01B.folder, Anm::SLPL01B.txt, Anm::SLPL01B.offset)},
    };

    int total = 0;
    for (auto& e : loaded) total += e.scriptCount;
    m_Vms.resize(total);

    m_DifficultyItemVms.resize(SELECT_DIFFICULTY_COUNT);

    m_CharacterItemVms.resize(SELECT_CHARACTER_COUNT);
    for (int i = 0; i < SELECT_CHARACTER_COUNT; i++) {
        m_CharacterItemVms[i].resize(SELECT_SPELLCARD_COUNT);
    }

    m_SpellCardItemVms.resize(SELECT_CHARACTER_COUNT);
    for (int i = 0; i < SELECT_CHARACTER_COUNT; i++) {
        m_SpellCardItemVms[i].resize(SELECT_SPELLCARD_COUNT);
    }

    // Init each VM and GameObject
    int vmIdx            = 0;
    int difficultyVmIdx  = 0;
    int character00VmIdx = 0;
    int character01VmIdx = 0;
    for (auto& e : loaded) {
        for (int i = 0; i < e.scriptCount; i++, vmIdx++) {
            m_Anm.SetScript(m_Vms[vmIdx], e.entry->offset + i, e.entry->offset);

            m_Renderer.AddChild(m_Vms[vmIdx].obj);

            if (e.entry == &Anm::SELECT01 ||
                e.entry == &Anm::SELECT02) {  // skip idx of 0 since it is difficulty select title
                if (difficultyVmIdx == 0) {
                    m_DifficultyTitleVm = &m_Vms[vmIdx];
                } else {
                    m_DifficultyItemVms[difficultyVmIdx - 1] = &m_Vms[vmIdx];
                }
                difficultyVmIdx++;
            } else if (e.entry == &Anm::SLPL00A || e.entry == &Anm::SLPL00B) {
                m_CharacterItemVms[0][character00VmIdx]         = &m_Vms[vmIdx];
                m_CharacterItemVms[0][character00VmIdx]->zIndex = 1.5f;
                character00VmIdx++;
            } else if (e.entry == &Anm::SLPL01A || e.entry == &Anm::SLPL01B) {
                m_CharacterItemVms[1][character01VmIdx]         = &m_Vms[vmIdx];
                m_CharacterItemVms[1][character01VmIdx]->zIndex = 1.5f;
                character01VmIdx++;
            } else if (e.entry == &Anm::SELECT03) {
                if (i == 0) {
                    m_CharacterTitleVm         = &m_Vms[vmIdx];
                    m_CharacterTitleVm->zIndex = 1.6f;
                } else if (i == 1) {
                    m_SpellCardTitleVm         = &m_Vms[vmIdx];
                    m_SpellCardTitleVm->zIndex = 1.6f;
                }
            } else if (e.entry == &Anm::SELECT04) {
                if (i < 2) {
                    m_SpellCardItemVms[0][i] = &m_Vms[vmIdx];
                    m_SpellCardItemVms[0][i]->zIndex =
                        1.6f;  // spell card items should be above character items
                } else {
                    m_SpellCardItemVms[1][i - 2]         = &m_Vms[vmIdx];
                    m_SpellCardItemVms[1][i - 2]->zIndex = 1.6f;
                }
            }
        }
    }
    m_EnterSelectBlackMask.Fade(30, 0.0f);  // start with black mask fully transparent
    m_Renderer.AddChild(m_EnterSelectBlackMask.GetObj());

    HandleInterruptEvent(SelectEvent::EnterDifficultySelect);
}

void Select::Update() {
    m_EnterSelectBlackMask.Update();
    switch (m_CurrentState) {
        case SelectState::Difficulty:
            if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
                m_SelectedDifficultyItemIdx =
                    (m_SelectedDifficultyItemIdx - 1 + SELECT_DIFFICULTY_COUNT) %
                    SELECT_DIFFICULTY_COUNT;
            } else if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
                m_SelectedDifficultyItemIdx =
                    (m_SelectedDifficultyItemIdx + 1) % SELECT_DIFFICULTY_COUNT;
            } else if (Util::Input::IsKeyDown(Util::Keycode::Z)) {
                m_CurrentState = SelectState::Character;
                HandleInterruptEvent(SelectEvent::EnterCharaSelect);

            } else if (Util::Input::IsKeyDown(Util::Keycode::X)) {
                HandleInterruptEvent(SelectEvent::ReturnTitle);
                m_EnterSelectBlackMask.Fade(30, 1.0f);  // fade to black before returning to title
                m_Quitting = true;                      // start quit timer and animation
            }
            m_SelectedDifficultyItem = static_cast<DifficultyItem>(m_SelectedDifficultyItemIdx);

            break;
        case SelectState::Character:
            if (Util::Input::IsKeyDown(Util::Keycode::LEFT)) {
                m_SelectedCharacterItemIdx =
                    (m_SelectedCharacterItemIdx - 1 + SELECT_CHARACTER_COUNT) %
                    SELECT_CHARACTER_COUNT;
                HandleInterruptEvent(SelectEvent::SwapCharaItemLeft);
            } else if (Util::Input::IsKeyDown(Util::Keycode::RIGHT)) {
                m_SelectedCharacterItemIdx =
                    (m_SelectedCharacterItemIdx + 1) % SELECT_CHARACTER_COUNT;
                HandleInterruptEvent(SelectEvent::SwapCharaItemRight);
            } else if (Util::Input::IsKeyDown(Util::Keycode::Z)) {
                m_CurrentState = SelectState::SpellCard;
                HandleInterruptEvent(SelectEvent::EnterSpellCardSelect);
            } else if (Util::Input::IsKeyDown(Util::Keycode::X)) {
                m_CurrentState = SelectState::Difficulty;
                HandleInterruptEvent(SelectEvent::ReturnDifficultySelect);
            }
            m_SelectedCharacterItem = static_cast<CharacterItem>(m_SelectedCharacterItemIdx);
            break;
        case SelectState::SpellCard:
            if (Util::Input::IsKeyDown(Util::Keycode::UP)) {
                m_SelectedSpellCardItemIdx =
                    (m_SelectedSpellCardItemIdx - 1 + SELECT_SPELLCARD_COUNT) %
                    SELECT_SPELLCARD_COUNT;
            } else if (Util::Input::IsKeyDown(Util::Keycode::DOWN)) {
                m_SelectedSpellCardItemIdx =
                    (m_SelectedSpellCardItemIdx + 1) % SELECT_SPELLCARD_COUNT;
            } else if (Util::Input::IsKeyDown(Util::Keycode::Z)) {
                m_Quitting = true;  // start quit timer and animation
            } else if (Util::Input::IsKeyDown(Util::Keycode::X)) {
                m_CurrentState = SelectState::Character;
                HandleInterruptEvent(SelectEvent::ReturnCharaSelect);
            }

            m_SelectedSpellCardItem = static_cast<SpellCardItem>(m_SelectedSpellCardItemIdx);
            break;
    }

    if (m_Quitting) {
        m_QuitTimer++;
        if (m_QuitTimer >= 30) {  // 30 frames of animation
            m_Done = true;
        }
    }

    m_Anm.UpdateObjects(m_Vms);

    // overwrite alpha value of difficulty items of the original instructions
    for (int i = 0; i < SELECT_DIFFICULTY_COUNT; i++) {
        if (m_CurrentState == SelectState::Difficulty) {
            if (i == m_SelectedDifficultyItemIdx) {
                m_DifficultyItemVms[i]->obj->SetAlpha(1.0f);
            } else {
                m_DifficultyItemVms[i]->obj->SetAlpha(0.5f);
            }
        }
    }

    if (m_CurrentState == SelectState::SpellCard) {
        if (m_SelectedCharacterItem == CharacterItem::Reimu) {
            for (int i = 0; i < SELECT_SPELLCARD_COUNT; i++) {
                if (i == m_SelectedSpellCardItemIdx) {
                    m_SpellCardItemVms[0][i]->alpha = 1.0f;
                } else {
                    m_SpellCardItemVms[0][i]->alpha = 0.5f;
                }
            }
        } else if (m_SelectedCharacterItem == CharacterItem::Marisa) {
            for (int i = 0; i < SELECT_SPELLCARD_COUNT; i++) {
                if (i == m_SelectedSpellCardItemIdx) {
                    m_SpellCardItemVms[1][i]->alpha = 1.0f;
                } else {
                    m_SpellCardItemVms[1][i]->alpha = 0.5f;
                }
            }
        }
    }

    m_Renderer.Update();
}

std::unique_ptr<Scene> Select::NextScene() {
    if (m_Done) {
        switch (m_CurrentState) {
            case SelectState::Difficulty:
                return std::make_unique<Title>();  // go back to title
            case SelectState::Character:
                break;
            case SelectState::SpellCard: {
                return std::make_unique<Stage1>(m_SelectedCharacterItem, m_SelectedSpellCardItem);
            }
        }
    }
    return nullptr;
}

void Select::HandleInterruptEvent(SelectEvent event) {
    switch (event) {
        case SelectEvent::ReturnDifficultySelect:
            m_Anm.SendInterrupt(*m_CharacterTitleVm,
                                SELECT_INTERRUPT_CHARA_TITLE_LEAVE_CHARA_SELECT);
            for (int i = 0; i < SELECT_CHARACTER_COUNT; i++) {
                for (int j = 0; j < SELECT_CHARACTER_PART_COUNT; j++) {
                    m_Anm.SendInterrupt(*m_CharacterItemVms[i][j],
                                        SELECT_INTERRUPT_CHARA_ITEM_RETURN_DIFFICULTY_SELECT);
                }
            }
            [[fallthrough]];
        case SelectEvent::EnterDifficultySelect:
            m_Anm.SendInterrupt(*m_DifficultyTitleVm, SELECT_INTERRUPT_ENTER_DIFFICULTY_SELECT);
            for (int i = 0; i < SELECT_DIFFICULTY_COUNT; i++) {
                m_Anm.SendInterrupt(*m_DifficultyItemVms[i],
                                    SELECT_INTERRUPT_ENTER_DIFFICULTY_SELECT);
            }
            break;

        case SelectEvent::ReturnCharaSelect:
            m_Anm.SendInterrupt(*m_SpellCardTitleVm,
                                SELECT_INTERRUPT_SPELLCARD_ALL_ITEM_LEAVE_SPELLCARD_SELECT);
            for (int i = 0; i < SELECT_SPELLCARD_COUNT; i++) {
                m_Anm.SendInterrupt(*m_SpellCardItemVms[m_SelectedCharacterItemIdx][i],
                                    SELECT_INTERRUPT_SPELLCARD_ALL_ITEM_LEAVE_SPELLCARD_SELECT);
            }
            [[fallthrough]];
        case SelectEvent::EnterCharaSelect:
            m_Anm.SendInterrupt(*m_DifficultyTitleVm,
                                SELECT_INTERRUPT_DIFFICULTY_TITLE_ENTER_CHARA_SELECT);
            for (int i = 0; i < SELECT_DIFFICULTY_COUNT; i++) {
                if (i == m_SelectedDifficultyItemIdx) {
                    m_Anm.SendInterrupt(
                        *m_DifficultyItemVms[i],
                        SELECT_INTERRUPT_SELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT);
                } else {
                    m_Anm.SendInterrupt(
                        *m_DifficultyItemVms[i],
                        SELECT_INTERRUPT_UNSELECTED_DIFFICULTY_ITEM_ENTER_CHARA_SELECT);
                }
            }

            m_Anm.SendInterrupt(*m_CharacterTitleVm, SELECT_INTERRUPT_ENTER_CHARA_SELECT);
            for (int i = 0; i < SELECT_CHARACTER_PART_COUNT; i++) {
                m_Anm.SendInterrupt(*m_CharacterItemVms[m_SelectedCharacterItemIdx][i],
                                    SELECT_INTERRUPT_ENTER_CHARA_SELECT);
            }
            break;
        case SelectEvent::ReturnTitle:
            m_Anm.SendInterrupt(*m_DifficultyTitleVm, SELECT_INTERRUPT_RETURN_TITLE);
            for (int i = 0; i < SELECT_DIFFICULTY_COUNT; i++) {
                m_Anm.SendInterrupt(*m_DifficultyItemVms[i], SELECT_INTERRUPT_RETURN_TITLE);
            }
            break;
        case SelectEvent::SwapCharaItemRight:
            for (int i = 0; i < SELECT_CHARACTER_COUNT; i++) {
                if (i == m_SelectedCharacterItemIdx) {
                    for (int j = 0; j < SELECT_CHARACTER_PART_COUNT; j++) {
                        m_Anm.SendInterrupt(*m_CharacterItemVms[i][j],
                                            SELECT_INTERRUPT_CHARA_ENTER_FROM_RIGHT);
                    }
                } else {
                    for (int j = 0; j < SELECT_CHARACTER_PART_COUNT; j++) {
                        m_Anm.SendInterrupt(*m_CharacterItemVms[i][j],
                                            SELECT_INTERRUPT_CHARA_LEAVE_FROM_LEFT);
                    }
                }
            }
            break;
        case SelectEvent::SwapCharaItemLeft:
            for (int i = 0; i < SELECT_CHARACTER_COUNT; i++) {
                if (i == m_SelectedCharacterItemIdx) {
                    for (int j = 0; j < SELECT_CHARACTER_PART_COUNT; j++) {
                        m_Anm.SendInterrupt(*m_CharacterItemVms[i][j],
                                            SELECT_INTERRUPT_CHARA_ENTER_FROM_LEFT);
                    }
                } else {
                    for (int j = 0; j < SELECT_CHARACTER_PART_COUNT; j++) {
                        m_Anm.SendInterrupt(*m_CharacterItemVms[i][j],
                                            SELECT_INTERRUPT_CHARA_LEAVE_FROM_RIGHT);
                    }
                }
            }
            break;
        case SelectEvent::EnterSpellCardSelect:
            m_Anm.SendInterrupt(*m_CharacterTitleVm,
                                SELECT_INTERRUPT_CHARA_TITLE_LEAVE_CHARA_SELECT);
            for (int i = 0; i < SELECT_CHARACTER_COUNT; i++) {
                if (i == m_SelectedCharacterItemIdx) {
                    for (int j = 0; j < SELECT_CHARACTER_PART_COUNT; j++) {
                        m_Anm.SendInterrupt(
                            *m_CharacterItemVms[i][j],
                            SELECT_INTERRUPT_SELECTED_CHARA_ITEM_ENTER_SPELLCARD_SELECT);
                    }
                }
            }

            m_Anm.SendInterrupt(*m_SpellCardTitleVm,
                                SELECT_INTERRUPT_SPELLCARD_ALL_ITEM_ENTER_SPELLCARD_SELECT);
            for (int i = 0; i < SELECT_SPELLCARD_COUNT; i++) {
                m_Anm.SendInterrupt(*m_SpellCardItemVms[m_SelectedCharacterItemIdx][i],
                                    SELECT_INTERRUPT_SPELLCARD_ALL_ITEM_ENTER_SPELLCARD_SELECT);
            }
            break;
    }
}