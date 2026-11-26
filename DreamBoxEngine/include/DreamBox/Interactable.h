
#pragma once

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>

namespace DreamBox
{
    class Interactable
    {
    public:
        explicit Interactable(
            const SDL_FRect& bounds
        )
            : m_bounds(bounds)
        {
        }

        virtual ~Interactable() = default;

        const SDL_FRect& GetBounds() const
        {
            return m_bounds;
        }

        // Can the player currently interact?
        virtual bool CanInteract() const = 0;

        // Perform the interaction.
        virtual void Interact(
            const SDL_FRect& playerHitbox
        ) = 0;

        // Does this object block movement?
        virtual bool IsSolid() const = 0;

        // Draw the object.
        virtual void Render(
            SDL_Renderer* renderer,
            bool playerNearby
        ) const = 0;

    protected:
        SDL_FRect m_bounds{};
    };
}
