
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
            bool selected,
            bool hovered = false
        ) const;

        const SDL_FRect& GetBounds() const;
        bool Contains(float x, float y) const;

        // used for testing
        static constexpr bool ContainsPoint(
            const SDL_FRect& bounds,
            float x,
            float y
        ) noexcept
        {
            return x >= bounds.x &&
                   x < bounds.x + bounds.w &&
                   y >= bounds.y &&
                   y < bounds.y + bounds.h;
        }

    private:
        SDL_FRect m_bounds{};

        std::unique_ptr<Text> m_label;
    };
}
