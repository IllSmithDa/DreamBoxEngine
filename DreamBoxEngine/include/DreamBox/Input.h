#pragma once

#include <SDL3/SDL.h>

namespace DreamBox
{
    class Input
    {
    public:
        Input() = default;

        bool IsKeyDown(SDL_Scancode key) const;
    };
}