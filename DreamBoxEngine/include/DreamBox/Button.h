
#pragma once

#include "DreamBox/Text.h"

#include <SDL3/SDL.h>

#include <memory>
#include <string>

namespace DreamBox
{
    class Button
    {
    public:
        Button(
            SDL_Renderer* renderer,
            const SDL_FRect& bounds,
            const std::string& fontPath,
            float fontSize,
            const std::string& label
        );

        void Render(
            SDL_Renderer* renderer,
            bool selected
        ) const;

        const SDL_FRect& GetBounds() const;

    private:
        SDL_FRect m_bounds{};

        std::unique_ptr<Text> m_label;
    };
}
