#pragma once

#include <SDL3/SDL_render.h>
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
            const SDL_FRect& playerHitbox
        ) const;

        bool CollidesWithSolid(
            const SDL_FRect& playerHitbox
        ) const;

    private:
        struct Chest
        {
            SDL_FRect bounds{};
            bool opened = false;
        };

        struct Door
        {
            SDL_FRect bounds{};
            bool open = false;
        };

        std::vector<Chest> m_chests;
        std::vector<Door> m_doors;

        bool m_wasInteractPressed = false;

        static float DistanceSquared(
            const SDL_FRect& first,
            const SDL_FRect& second
        );

        static bool IsNearby(
            const SDL_FRect& player,
            const SDL_FRect& object
        );
    };
}
