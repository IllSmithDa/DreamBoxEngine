
#pragma once
#include <optional>
#include "DreamBox/Interactable.h"
#include "DreamBox/Item.h"

namespace DreamBox
{
    class Chest final : public Interactable
    {
    public:
        explicit Chest(const SDL_FRect& bounds);

        bool CanInteract() const override;

        void Interact(
            const SDL_FRect& playerHitbox
        ) override;

        bool IsSolid() const override;

        void Render(
            SDL_Renderer* renderer,
            bool playerNearby
        ) const override;

        std::optional<ItemStack> GetUnclaimedLoot() const;
        void MarkLootClaimed();
    private:
        bool m_opened = false;
        ItemStack m_loot{
        ItemId::GoldCoin,
            25
        };
        
        bool m_lootClaimed = false;
    };
}
