
#include "DreamBox/InteractionSystem.h"
#include "DreamBox/Collision.h"

#include <SDL3/SDL_log.h>

namespace DreamBox
{
    void InteractionSystem::Initialize(
        const std::vector<SDL_FRect>& chestBounds,
        const std::vector<SDL_FRect>& doorBounds
    )
    {
        m_chests.clear();
        m_doors.clear();

        for (const SDL_FRect& bounds : chestBounds)
        {
            m_chests.push_back(Chest{bounds, false});
        }

        for (const SDL_FRect& bounds : doorBounds)
        {
            m_doors.push_back(Door{bounds, false});
        }
    }

    float InteractionSystem::DistanceSquared(
        const SDL_FRect& first,
        const SDL_FRect& second
    )
    {
        const float firstX = first.x + first.w / 2.0f;
        const float firstY = first.y + first.h / 2.0f;

        const float secondX = second.x + second.w / 2.0f;
        const float secondY = second.y + second.h / 2.0f;

        const float dx = firstX - secondX;
        const float dy = firstY - secondY;

        return dx * dx + dy * dy;
    }

    bool InteractionSystem::IsNearby(
        const SDL_FRect& player,
        const SDL_FRect& object
    )
    {
        constexpr float interactionRange = 70.0f;

        return DistanceSquared(player, object) <=
            interactionRange * interactionRange;
    }

    void InteractionSystem::Update(
        bool interactPressed,
        const SDL_FRect& playerHitbox
    )
    {
        const bool newPress =
            interactPressed && !m_wasInteractPressed;

        m_wasInteractPressed = interactPressed;

        if (!newPress)
        {
            return;
        }

        constexpr float interactionRange = 70.0f;
        const float maxDistanceSquared =
            interactionRange * interactionRange;

        // First, find the closest unopened chest.
        Chest* nearestChest = nullptr;
        float nearestDistance = maxDistanceSquared;

        for (Chest& chest : m_chests)
        {
            if (chest.opened)
            {
                continue;
            }

            const float distance =
                DistanceSquared(playerHitbox, chest.bounds);

            if (distance <= nearestDistance)
            {
                nearestDistance = distance;
                nearestChest = &chest;
            }
        }

        if (nearestChest)
        {
            nearestChest->opened = true;
            SDL_Log("Treasure chest opened!");
            return;
        }

        // Otherwise, find the closest door.
        Door* nearestDoor = nullptr;
        nearestDistance = maxDistanceSquared;

        for (Door& door : m_doors)
        {
            const float distance =
                DistanceSquared(playerHitbox, door.bounds);

            if (distance <= nearestDistance)
            {
                nearestDistance = distance;
                nearestDoor = &door;
            }
        }

        if (!nearestDoor)
        {
            return;
        }

        if (!nearestDoor->open)
        {
            nearestDoor->open = true;
            SDL_Log("Door opened");
        }
        else if (!Collision::Intersects(
                     playerHitbox,
                     nearestDoor->bounds))
        {
            nearestDoor->open = false;
            SDL_Log("Door closed");
        }
        else
        {
            SDL_Log("Cannot close door: player is inside");
        }
    }

    bool InteractionSystem::CollidesWithSolid(
        const SDL_FRect& playerHitbox
    ) const
    {
        for (const Chest& chest : m_chests)
        {
            if (Collision::Intersects(
                    playerHitbox,
                    chest.bounds))
            {
                return true;
            }
        }

        for (const Door& door : m_doors)
        {
            if (!door.open &&
                Collision::Intersects(
                    playerHitbox,
                    door.bounds))
            {
                return true;
            }
        }

        return false;
    }

    void InteractionSystem::Render(
        SDL_Renderer* renderer,
        const SDL_FRect& playerHitbox
    ) const
    {
        if (!renderer)
        {
            return;
        }

        // ----------------------------------
        // Draw treasure chests
        // ----------------------------------
        for (const Chest& chest : m_chests)
        {
            SDL_FRect visual = chest.bounds;

            visual.x += 5.0f;
            visual.y += 8.0f;
            visual.w -= 10.0f;
            visual.h -= 12.0f;

            if (chest.opened)
            {
                SDL_SetRenderDrawColor(
                    renderer, 95, 65, 40, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(
                    renderer, 165, 100, 45, 255);
            }

            SDL_RenderFillRect(renderer, &visual);

            SDL_SetRenderDrawColor(
                renderer, 40, 25, 15, 255);

            SDL_RenderRect(renderer, &visual);

            if (!chest.opened &&
                IsNearby(playerHitbox, chest.bounds))
            {
                SDL_SetRenderDrawColor(
                    renderer, 255, 220, 70, 255);

                SDL_RenderRect(renderer, &chest.bounds);
            }
        }

        // ----------------------------------
        // Draw doors
        // ----------------------------------
        for (const Door& door : m_doors)
        {
            SDL_FRect visual = door.bounds;

            visual.x += 6.0f;
            visual.y += 2.0f;
            visual.w -= 12.0f;
            visual.h -= 4.0f;

            if (door.open)
            {
                visual.x += 12.0f;
                visual.w -= 12.0f;

                SDL_SetRenderDrawColor(
                    renderer, 90, 60, 35, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(
                    renderer, 145, 90, 45, 255);
            }

            SDL_RenderFillRect(renderer, &visual);

            SDL_SetRenderDrawColor(
                renderer, 40, 25, 15, 255);

            SDL_RenderRect(renderer, &visual);

            if (!door.open &&
                IsNearby(playerHitbox, door.bounds))
            {
                SDL_SetRenderDrawColor(
                    renderer, 255, 220, 70, 255);

                SDL_RenderRect(renderer, &door.bounds);
            }
        }
    }
}
