#pragma once

#include <SDL3/SDL.h>
#include <array>

namespace DreamBox
{
    class Input
    {
    public:
        Input() = default;

        void Update();

        bool IsKeyDown(SDL_Scancode key) const;
        bool IsKeyPressed(SDL_Scancode key) const;

    private:
        std::array<bool, SDL_SCANCODE_COUNT> m_currentKeys{};
        std::array<bool, SDL_SCANCODE_COUNT> m_previousKeys{};
    };
}