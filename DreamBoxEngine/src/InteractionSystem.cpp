
#include "DreamBox/InteractionSystem.h"

#include "DreamBox/Chest.h"
#include "DreamBox/Door.h"
#include "DreamBox/Collision.h"

#include <SDL3/SDL_log.h>

#include <memory>

namespace DreamBox
{
    void InteractionSystem::Initialize(
        const std::vector<SDL_FRect>& chestBounds,
        const std::vector<SDL_FRect>& doorBounds
    )
    {
        m_objects.clear();

        m_objects.reserve(
            chestBounds.size() + doorBounds.size()
        );

        for (const SDL_FRect& bounds : chestBounds)
        {
            m_objects.push_back(
                std::make_unique<Chest>(bounds)
            );
        }

        for (const SDL_FRect& bounds : doorBounds)
        {
            m_objects.push_back(
                std::make_unique<Door>(bounds)
            );
        }

        SDL_Log(
            "InteractionSystem: %zu objects loaded",
            m_objects.size()
        );
    }

    float InteractionSystem::DistanceSquared(
        const SDL_FRect& first,
        const SDL_FRect& second
    )
    {
        const float firstX =
            first.x + first.w / 2.0f;

        const float firstY =
            first.y + first.h / 2.0f;

        const float secondX =
            second.x + second.w / 2.0f;

        const float secondY =
            second.y + second.h / 2.0f;

        const float dx = firstX - secondX;
        const float dy = firstY - secondY;

        return dx * dx + dy * dy;
    }

    bool InteractionSystem::IsNearby(
        const SDL_FRect& player,
        const SDL_FRect& object
    )
    {
        constexpr float range = 70.0f;

        return DistanceSquared(player, object) <=
            range * range;
    }

    void InteractionSystem::Update(
        bool interactPressed,
        const SDL_FRect& playerHitbox
    )
    {
        const bool newPress =
            interactPressed && !m_wasInteractPressed;

        // Important: update this every frame.
        m_wasInteractPressed = interactPressed;

        if (!newPress)
        {
            return;
        }

        constexpr float range = 70.0f;

        const float maxDistanceSquared =
            range * range;

        Interactable* nearest = nullptr;
        float nearestDistanceSquared =
            maxDistanceSquared;

        for (const auto& object : m_objects)
        {
            if (!object->CanInteract())
            {
                continue;
            }

            const float distance =
                DistanceSquared(
                    playerHitbox,
                    object->GetBounds()
                );

            if (distance <= maxDistanceSquared &&
                (!nearest ||
                 distance < nearestDistanceSquared))
            {
                nearestDistanceSquared = distance;
                nearest = object.get();
            }
        }

        if (nearest)
        {
            nearest->Interact(playerHitbox);
        }
    }

    bool InteractionSystem::CollidesWithSolid(
        const SDL_FRect& playerHitbox
    ) const
    {
        for (const auto& object : m_objects)
        {
            if (object->IsSolid() &&
                Collision::Intersects(
                    playerHitbox,
                    object->GetBounds()
                ))
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

        for (const auto& object : m_objects)
        {
            const bool nearby =
                IsNearby(
                    playerHitbox,
                    object->GetBounds()
                );

            object->Render(
                renderer,
                nearby
            );
        }
    }
}
