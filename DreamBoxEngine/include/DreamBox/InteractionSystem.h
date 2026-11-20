
#pragma once

#include "DreamBox/Interactable.h"
#include <cstddef>
#include <optional>
#include <memory>
#include <vector>

namespace DreamBox
{
    class InteractionSystem
    {
    public:
        void Initialize(
            const std::vector<SDL_FRect>& chestBounds,
            const std::vector<SDL_FRect>& doorBounds
        );

        void Update(
            bool interactPressed,
            const SDL_FRect& playerHitbox
        );

        void Render(
            SDL_Renderer* renderer,
            const SDL_FRect& playerHitbox,
            const std::optional<SDL_FPoint>& mousePosition
        ) const;

        bool CollidesWithSolid(
            const SDL_FRect& playerHitbox
        ) const;

        bool HandlePointerPress(
            const SDL_FPoint& point,
            const SDL_FRect& playerHitbox
        );

        void ClearSelection()
        {
            m_selectedIndex.reset();
        }
    private:
        std::vector<std::unique_ptr<Interactable>>
            m_objects;

        bool m_wasInteractPressed = false;

        static float DistanceSquared(
            const SDL_FRect& first,
            const SDL_FRect& second
        );

        static bool IsNearby(
            const SDL_FRect& player,
            const SDL_FRect& object
        );

        std::optional<std::size_t> m_selectedIndex;
    };
}
