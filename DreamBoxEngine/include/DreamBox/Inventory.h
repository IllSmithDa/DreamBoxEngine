
#pragma once

#include "DreamBox/Item.h"

#include <cstddef>
#include <vector>

namespace DreamBox
{
    class Inventory
    {
    public:
        explicit Inventory(
            std::size_t maxSlots = 12,
            int maxStackSize = 99
        );

        bool Add(ItemId id, int quantity = 1);

        bool Remove(ItemId id, int quantity = 1);

        int Count(ItemId id) const;

        const std::vector<ItemStack>&
            GetStacks() const;

        std::size_t GetMaxSlots() const;

    private:
        std::vector<ItemStack> m_stacks;

        std::size_t m_maxSlots;
        int m_maxStackSize;
    };
}
