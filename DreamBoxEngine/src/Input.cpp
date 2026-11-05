#include "DreamBox/Input.h"

namespace DreamBox
{
    void Input::Update()
    {
        m_previousKeys = m_currentKeys;

        const bool* keyboardState =
            SDL_GetKeyboardState(nullptr);

        for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
        {
            m_currentKeys[i] = keyboardState[i];
        }
    }


    bool Input::IsKeyDown(SDL_Scancode key) const
    {
        return m_currentKeys[key];
    }


    bool Input::IsKeyPressed(SDL_Scancode key) const
    {
        return m_currentKeys[key] &&
               !m_previousKeys[key];
    }
}