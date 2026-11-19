
#pragma once

#include "DreamBox/Interactable.h"

namespace DreamBox
{
    class Door final : public Interactable
    {
    public:
        explicit Door(const SDL_FRect& bounds);

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
        bool m_open = false;
    };
}
