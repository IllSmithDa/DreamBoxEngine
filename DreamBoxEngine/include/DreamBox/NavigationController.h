
#pragma once

#include <SDL3/SDL_rect.h>
#include <functional>
#include <cstddef>
#include <optional>
#include <vector>

namespace DreamBox
{
    class NavigationController
    {
    public:
        void Begin(
            std::vector<SDL_FPoint> waypoints,
            SDL_FPoint destination
        );

        void Cancel();

        bool HasPath() const;

        const std::optional<SDL_FPoint>&
            GetDestination() const;

        SDL_FPoint CalculateMovement(
            const SDL_FRect& playerHitbox,
            float deltaTime
        );

        void UpdateAfterMovement(
            const SDL_FPoint& requestedMovement,
            const SDL_FPoint& actualMovement,
            const SDL_FRect& playerHitbox
        );

        using IsBlocked =
            std::function<bool(const SDL_FRect&)>;

        bool TryNavigateTo(
            SDL_FPoint destination,
            const SDL_FRect& actorHitbox,
            int columns,
            int rows,
            float tileSize,
            const IsBlocked& isBlocked
        );


        bool TryNavigateWithinRange(
            const SDL_FRect& targetBounds,
            const SDL_FRect& actorHitbox,
            float interactionRange,
            int columns,
            int rows,
            float tileSize,
            const IsBlocked& isBlocked
        );

    private:
        std::optional<SDL_FPoint> m_destination;

        std::vector<SDL_FPoint> m_waypoints;

        std::size_t m_nextWaypoint = 0;

        static constexpr float MoveSpeed = 200.0f;
        static constexpr float ArrivalTolerance = 1.0f;
    };
}
