
#pragma once

namespace DreamBox
{
    enum class PauseMenuOption
    {
        Resume,
        MainMenu,
        Count
    };

    class PauseMenuModel
    {
    public:
        void Reset()
        {
            m_selected = PauseMenuOption::Resume;
        }

        PauseMenuOption GetSelected() const
        {
            return m_selected;
        }

        void Select(PauseMenuOption option)
        {
            if (option != PauseMenuOption::Count)
            {
                m_selected = option;
            }
        }

        void MoveSelection(int direction)
        {
            constexpr int optionCount =
                static_cast<int>(PauseMenuOption::Count);

            const int current =
                static_cast<int>(m_selected);

            int next =
                (current + direction) % optionCount;

            if (next < 0)
            {
                next += optionCount;
            }

            m_selected =
                static_cast<PauseMenuOption>(next);
        }

    private:
        PauseMenuOption m_selected =
            PauseMenuOption::Resume;
    };
}
