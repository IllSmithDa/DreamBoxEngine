
#include "DreamBox/NavigationController.h"
#include "DreamBox/Pathfinder.h"
#include <SDL3/SDL_log.h>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

namespace DreamBox
{
    void NavigationController::Begin(
        std::vector<SDL_FPoint> waypoints,
        SDL_FPoint destination
    )
    {
        Cancel();

        if (waypoints.empty())
        {
            return;
        }

        m_waypoints = std::move(waypoints);
        m_destination = destination;
        m_nextWaypoint = 0;
    }

    void NavigationController::Cancel()
    {
        m_destination.reset();
        m_waypoints.clear();
        m_nextWaypoint = 0;
    }

    bool NavigationController::HasPath() const
    {
        return m_destination.has_value();
    }

    const std::optional<SDL_FPoint>&
    NavigationController::GetDestination() const
    {
        return m_destination;
    }

    SDL_FPoint NavigationController::CalculateMovement(
        const SDL_FRect& playerHitbox,
        float deltaTime
    )
    {
        if (!HasPath())
        {
            return {0.0f, 0.0f};
        }

        const SDL_FPoint center{
            playerHitbox.x + playerHitbox.w / 2.0f,
            playerHitbox.y + playerHitbox.h / 2.0f
        };

        while (m_nextWaypoint < m_waypoints.size())
        {
            const SDL_FPoint& waypoint =
                m_waypoints[m_nextWaypoint];

            const float dx = waypoint.x - center.x;
            const float dy = waypoint.y - center.y;

            const float distance =
                std::sqrt(dx * dx + dy * dy);

            if (distance <= ArrivalTolerance)
            {
                ++m_nextWaypoint;
                continue;
            }

            const float step = std::min(
                MoveSpeed * std::max(deltaTime, 0.0f),
                distance
            );

            return {
                dx / distance * step,
                dy / distance * step
            };
        }

        Cancel();
        return {0.0f, 0.0f};
    }

    void NavigationController::UpdateAfterMovement(
        const SDL_FPoint& requestedMovement,
        const SDL_FPoint& actualMovement,
        const SDL_FRect& playerHitbox
    )
    {
        if (!HasPath())
        {
            return;
        }

        constexpr float epsilon = 0.0001f;

        const bool movementRequested =
            std::fabs(requestedMovement.x) > epsilon ||
            std::fabs(requestedMovement.y) > epsilon;

        const bool actuallyMoved =
            std::fabs(actualMovement.x) > epsilon ||
            std::fabs(actualMovement.y) > epsilon;

        // Stop attempting a route if collision
        // prevents the player from moving.
        if (movementRequested && !actuallyMoved)
        {
            Cancel();
            return;
        }

        if (m_nextWaypoint >= m_waypoints.size())
        {
            Cancel();
            return;
        }

        const SDL_FPoint& waypoint =
            m_waypoints[m_nextWaypoint];

        const float centerX =
            playerHitbox.x + playerHitbox.w / 2.0f;

        const float centerY =
            playerHitbox.y + playerHitbox.h / 2.0f;

        const float dx = waypoint.x - centerX;
        const float dy = waypoint.y - centerY;

        const float distance =
            std::sqrt(dx * dx + dy * dy);

        if (distance <= ArrivalTolerance)
        {
            ++m_nextWaypoint;

            if (m_nextWaypoint >= m_waypoints.size())
            {
                Cancel();
            }
        }
    }
}
