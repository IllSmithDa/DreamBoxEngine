
#include "DreamBox/PickupSystem.h"

#include <algorithm>
#include <utility>

namespace DreamBox
{
    void PickupSystem::SetPickups(
        std::vector<GroundPickup> pickups
    )
    {
        m_pickups = std::move(pickups);
    }

    std::optional<SDL_FPoint>
    PickupSystem::GetClickedDestination(
        const SDL_FPoint& point
    ) const
    {
        for (const GroundPickup& pickup : m_pickups)
        {
            if (pickup.collected)
            {
                continue;
            }

            const SDL_FRect& bounds = pickup.bounds;

            const bool clicked =
                point.x >= bounds.x &&
                point.x < bounds.x + bounds.w &&
                point.y >= bounds.y &&
                point.y < bounds.y + bounds.h;

            if (clicked)
            {
                return SDL_FPoint{
                    bounds.x + bounds.w / 2.0f,
                    bounds.y + bounds.h / 2.0f
                };
            }
        }

        return std::nullopt;
    }

    bool PickupSystem::IsWithinPickupRange(
        const SDL_FRect& playerHitbox,
        const SDL_FRect& pickupBounds
    )
    {
        // Distance between the two rectangles.
        // Overlapping rectangles have a gap of zero.

        const float horizontalGap = std::max(
            0.0f,
            std::max(
                pickupBounds.x -
                    (playerHitbox.x + playerHitbox.w),
                playerHitbox.x -
                    (pickupBounds.x + pickupBounds.w)
            )
        );

        const float verticalGap = std::max(
            0.0f,
            std::max(
                pickupBounds.y -
                    (playerHitbox.y + playerHitbox.h),
                playerHitbox.y -
                    (pickupBounds.y + pickupBounds.h)
            )
        );

        const float distanceSquared =
            horizontalGap * horizontalGap +
            verticalGap * verticalGap;

        return distanceSquared <=
            PickupRange * PickupRange;
    }

    std::vector<ItemStack> PickupSystem::CollectNearby(
        const SDL_FRect& playerHitbox,
        Inventory& inventory
    )
    {
        std::vector<ItemStack> collected;

        for (GroundPickup& pickup : m_pickups)
        {
            if (pickup.collected ||
                pickup.loot.quantity <= 0)
            {
                continue;
            }

            if (!IsWithinPickupRange(
                    playerHitbox,
                    pickup.bounds))
            {
                continue;
            }

            // Inventory::Add guarantees that a
            // failed addition changes nothing.
            if (!inventory.Add(
                    pickup.loot.id,
                    pickup.loot.quantity))
            {
                // Leave the pickup on the ground.
                continue;
            }

            // Only remove it after collection succeeds.
            pickup.collected = true;

            collected.push_back(pickup.loot);
        }

        return collected;
    }

    const std::vector<GroundPickup>&
    PickupSystem::GetPickups() const
    {
        return m_pickups;
    }
}
