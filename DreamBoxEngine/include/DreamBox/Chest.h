
#pragma once

#include "DreamBox/Interactable.h"

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

    private:
        bool m_opened = false;
    };
}
