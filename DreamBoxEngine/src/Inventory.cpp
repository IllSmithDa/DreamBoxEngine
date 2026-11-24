
#include "DreamBox/Inventory.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace DreamBox
{
    Inventory::Inventory(
        std::size_t maxSlots,
        int maxStackSize
    )
        : m_maxSlots(maxSlots),
          m_maxStackSize(maxStackSize)
    {
        if (maxStackSize <= 0)
        {
            throw std::invalid_argument(
                "Maximum stack size must be positive"
            );
        }
    }

    bool Inventory::Add(
        ItemId id,
        int quantity
    )
    {
        if (quantity <= 0)
        {
            return false;
        }

        // Work on a copy so a failed operation
        // never partially modifies the inventory.
        auto updated = m_stacks;
        int remaining = quantity;

        // Fill existing stacks first.
        for (ItemStack& stack : updated)
        {
            if (stack.id != id)
            {
                continue;
            }

            const int space =
                m_maxStackSize - stack.quantity;

            const int amount =
                std::min(space, remaining);

            stack.quantity += amount;
            remaining -= amount;

            if (remaining == 0)
            {
                m_stacks = std::move(updated);
                return true;
            }
        }

        // Create additional stacks if needed.
        while (remaining > 0)
        {
            if (updated.size() >= m_maxSlots)
            {
                return false;
            }

            const int amount =
                std::min(remaining, m_maxStackSize);

            updated.push_back({id, amount});
            remaining -= amount;
        }

        m_stacks = std::move(updated);
        return true;
    }

    bool Inventory::Remove(
        ItemId id,
        int quantity
    )
    {
        if (quantity <= 0 || Count(id) < quantity)
        {
            return false;
        }

        int remaining = quantity;

        for (auto it = m_stacks.begin();
             it != m_stacks.end() &&
             remaining > 0;)
        {
            if (it->id != id)
            {
                ++it;
                continue;
            }

            const int amount =
                std::min(it->quantity, remaining);

            it->quantity -= amount;
            remaining -= amount;

            if (it->quantity == 0)
            {
                it = m_stacks.erase(it);
            }
            else
            {
                ++it;
            }
        }

        return true;
    }

    int Inventory::Count(ItemId id) const
    {
        int total = 0;

        for (const ItemStack& stack : m_stacks)
        {
            if (stack.id == id)
            {
                total += stack.quantity;
            }
        }

        return total;
    }

    const std::vector<ItemStack>&
    Inventory::GetStacks() const
    {
        return m_stacks;
    }

    std::size_t Inventory::GetMaxSlots() const
    {
        return m_maxSlots;
    }
}
