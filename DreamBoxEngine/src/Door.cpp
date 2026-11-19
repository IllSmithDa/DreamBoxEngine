
#include "DreamBox/Door.h"
#include "DreamBox/Collision.h"

#include <SDL3/SDL_log.h>

namespace DreamBox
{
    Door::Door(const SDL_FRect& bounds)
        : Interactable(bounds)
    {
    }

    bool Door::CanInteract() const
    {
        return true;
    }

    void Door::Interact(
        const SDL_FRect& playerHitbox
    )
    {
        if (!m_open)
        {
            m_open = true;
            SDL_Log("Door opened");
            return;
        }

        // Prevent closing on the player.
        if (Collision::Intersects(
                playerHitbox,
                m_bounds))
        {
            SDL_Log(
                "Cannot close door: player is inside"
            );
            return;
        }

        m_open = false;
        SDL_Log("Door closed");
    }

    bool Door::IsSolid() const
    {
        return !m_open;
    }

    void Door::Render(
        SDL_Renderer* renderer,
        bool playerNearby
    ) const
    {
        if (!renderer)
        {
            return;
        }

        SDL_FRect visual = m_bounds;

        visual.x += 6.0f;
        visual.y += 2.0f;
        visual.w -= 12.0f;
        visual.h -= 4.0f;

        if (m_open)
        {
            visual.x += 12.0f;
            visual.w -= 12.0f;

            SDL_SetRenderDrawColor(
                renderer, 90, 60, 35, 255
            );
        }
        else
        {
            SDL_SetRenderDrawColor(
                renderer, 145, 90, 45, 255
            );
        }

        SDL_RenderFillRect(renderer, &visual);

        SDL_SetRenderDrawColor(
            renderer, 40, 25, 15, 255
        );

        SDL_RenderRect(renderer, &visual);

        if (!m_open && playerNearby)
        {
            SDL_SetRenderDrawColor(
                renderer, 255, 220, 70, 255
            );

            SDL_RenderRect(renderer, &m_bounds);
        }
    }
}
