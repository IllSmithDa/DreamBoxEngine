
#pragma once

namespace DreamBox
{
    // A unique identifier for each item type.
    enum class ItemId
    {
        GoldCoin,
        HealthPotion,
        BronzeKey
    };

    // One inventory stack of a particular item.
    struct ItemStack
    {
        ItemId id = ItemId::GoldCoin;
        int quantity = 0;
    };
}
