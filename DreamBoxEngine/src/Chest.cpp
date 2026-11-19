
#include "DreamBox/Chest.h"

#include <SDL3/SDL_log.h>

namespace DreamBox
{
    Chest::Chest(const SDL_FRect& bounds)
        : Interactable(bounds)
    {
    }

    bool Chest::CanInteract() const
    {
        return !m_opened;
    }

    void Chest::Interact(
        const SDL_FRect& /*playerHitbox*/
    )
    {
        if (m_opened)
        {
            return;
        }

        m_opened = true;
        SDL_Log("Treasure chest opened!");
    }

    bool Chest::IsSolid() const
    {
        return true;
    }

    void Chest::Render(
        SDL_Renderer* renderer,
        bool playerNearby
    ) const
    {
        if (!renderer)
        {
            return;
        }

        SDL_FRect visual = m_bounds;

        visual.x += 5.0f;
        visual.y += 8.0f;
        visual.w -= 10.0f;
        visual.h -= 12.0f;

        if (m_opened)
        {
            SDL_SetRenderDrawColor(
                renderer, 95, 65, 40, 255
            );
        }
        else
        {
            SDL_SetRenderDrawColor(
                renderer, 165, 100, 45, 255
            );
        }

        SDL_RenderFillRect(renderer, &visual);

        SDL_SetRenderDrawColor(
            renderer, 40, 25, 15, 255
        );

        SDL_RenderRect(renderer, &visual);

        if (!m_opened && playerNearby)
        {
            SDL_SetRenderDrawColor(
                renderer, 255, 220, 70, 255
            );

            SDL_RenderRect(renderer, &m_bounds);
        }
    }
}
