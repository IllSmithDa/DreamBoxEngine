
#pragma once

#include "DreamBox/GroundPickup.h"
#include "DreamBox/Inventory.h"

#include <SDL3/SDL_rect.h>

#include <optional>
#include <vector>

namespace DreamBox
{
    class PickupSystem
    {
    public:
        void SetPickups(
            std::vector<GroundPickup> pickups
        );

        // Returns the center of a clicked,
        // uncollected ground item.
        std::optional<SDL_FPoint>
        GetClickedDestination(
            const SDL_FPoint& point
        ) const;

        // Attempts to collect all nearby items.
        // Returns the rewards successfully added.
        std::vector<ItemStack> CollectNearby(
            const SDL_FRect& playerHitbox,
            Inventory& inventory
        );

        const std::vector<GroundPickup>&
        GetPickups() const;

    private:
        static bool IsWithinPickupRange(
            const SDL_FRect& playerHitbox,
            const SDL_FRect& pickupBounds
        );

        std::vector<GroundPickup> m_pickups;

        static constexpr float PickupRange = 8.0f;
    };
}
