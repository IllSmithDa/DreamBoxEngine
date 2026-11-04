#include "DreamBox/Input.h"

namespace DreamBox
{
    bool Input::IsKeyDown(SDL_Scancode key) const
    {
        const bool* keyboardState =
            SDL_GetKeyboardState(nullptr);

        return keyboardState[key];
    }
}