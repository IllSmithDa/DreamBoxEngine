
#pragma once

#include "DreamBox/Item.h"

#include <SDL3/SDL_rect.h>

namespace DreamBox
{
    struct GroundPickup
    {
        // What item will be collected?
        ItemStack loot{
            ItemId::GoldCoin,
            5
        };

        // Position and size in world coordinates.
        SDL_FRect bounds{};

        // Once collected, the pickup is inactive.
        bool collected = false;
    };
}
