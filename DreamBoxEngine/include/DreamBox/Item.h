
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

    inline constexpr const char* GetItemName(ItemId id)
    {
        switch (id)
        {
        case ItemId::GoldCoin:
            return "Gold Coins";
        
        case ItemId::HealthPotion:
            return "Health Potions";
        
        case ItemId::BronzeKey:
            return "Bronze Keys";
        
        default:
            return "Unknown Item";
        }
    }

}
