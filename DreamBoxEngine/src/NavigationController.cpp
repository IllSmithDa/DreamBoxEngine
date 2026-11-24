
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


    bool NavigationController::TryNavigateTo(
        SDL_FPoint destination,
        const SDL_FRect& actorHitbox,
        int columns,
        int rows,
        float tileSize,
        const IsBlocked& isBlocked
    )
    {
        // New navigation replaces any previous route.
        Cancel();

        if (columns <= 0 || rows <= 0 ||
            tileSize <= 0.0f ||
            actorHitbox.w <= 0.0f ||
            actorHitbox.h <= 0.0f ||
            !isBlocked)
        {
            return false;
        }

        const float worldWidth = columns * tileSize;
        const float worldHeight = rows * tileSize;

        // ----------------------------------
        // 1. Check whether the actor fits
        // ----------------------------------

        const auto canStandAt =
            [&](SDL_FPoint center) -> bool
        {
            const SDL_FRect candidate{
                center.x - actorHitbox.w / 2.0f,
                center.y - actorHitbox.h / 2.0f,
                actorHitbox.w,
                actorHitbox.h
            };

            if (candidate.x < 0.0f ||
                candidate.y < 0.0f ||
                candidate.x + candidate.w > worldWidth ||
                candidate.y + candidate.h > worldHeight)
            {
                return false;
            }

            return !isBlocked(candidate);
        };

        if (!canStandAt(destination))
        {
            return false;
        }

        // ----------------------------------
        // 2. Convert world positions to grid
        // ----------------------------------

        const SDL_FPoint startCenter{
            actorHitbox.x + actorHitbox.w / 2.0f,
            actorHitbox.y + actorHitbox.h / 2.0f
        };

        const GridPoint start{
            static_cast<int>(startCenter.x / tileSize),
            static_cast<int>(startCenter.y / tileSize)
        };

        const GridPoint goal{
            static_cast<int>(destination.x / tileSize),
            static_cast<int>(destination.y / tileSize)
        };

        const auto tileCenter =
            [tileSize](GridPoint cell) -> SDL_FPoint
        {
            return {
                (static_cast<float>(cell.x) + 0.5f) *
                    tileSize,
                (static_cast<float>(cell.y) + 0.5f) *
                    tileSize
            };
        };

        // ----------------------------------
        // 3. Check grid walkability
        // ----------------------------------

        const auto isWalkable =
            [&](int column, int row) -> bool
        {
            // The actor already occupies its starting cell.
            if (column == start.x && row == start.y)
            {
                return true;
            }
        
            // The destination may be valid even when
            // the center of its grid cell is blocked.
            if (column == goal.x && row == goal.y)
            {
                return canStandAt(destination);
            }
        
            return canStandAt(
                tileCenter({column, row})
            );
        };


        // ----------------------------------
        // 4. Calculate A* route
        // ----------------------------------

        const std::vector<GridPoint> path =
            Pathfinder::FindPath(
                start,
                goal,
                columns,
                rows,
                isWalkable
            );

        if (path.empty())
        {
            return false;
        }

        // ----------------------------------
        // 5. Convert path into waypoints
        // ----------------------------------

        std::vector<SDL_FPoint> waypoints;
        waypoints.reserve(path.size() + 1);

        const SDL_FPoint firstTileCenter =
            tileCenter(start);

        if (canStandAt(firstTileCenter))
        {
            waypoints.push_back(firstTileCenter);
        }

        // Skip starting grid node.

        for (std::size_t i = 1; i < path.size(); ++i)
        {
            const SDL_FPoint center =
                tileCenter(path[i]);
        
            const bool isFinalCell =
                i + 1 == path.size();
        
            // Avoid forcing the actor through a blocked
            // tile center when the precise goal is valid.
            if (isFinalCell && !canStandAt(center))
            {
                continue;
            }
        
            waypoints.push_back(center);
        }

        // The last waypoint remains the exact destination.
        waypoints.push_back(destination);

        SDL_Log(
            "A*: route created with %zu grid nodes "
            "and %zu waypoints",
            path.size(),
            waypoints.size()
        );

        // ----------------------------------
        // 6. Begin navigation
        // ----------------------------------

        Begin(
            std::move(waypoints),
            destination
        );

        return true;
    }

    bool NavigationController::TryNavigateWithinRange(
        const SDL_FRect& targetBounds,
        const SDL_FRect& actorHitbox,
        float interactionRange,
        int columns,
        int rows,
        float tileSize,
        const IsBlocked& isBlocked
    )
    {
        // Replace any existing navigation.
        Cancel();

        if (columns <= 0 || rows <= 0 ||
            tileSize <= 0.0f ||
            interactionRange < 0.0f ||
            actorHitbox.w <= 0.0f ||
            actorHitbox.h <= 0.0f ||
            !isBlocked)
        {
            return false;
        }

        const float worldWidth =
            columns * tileSize;

        const float worldHeight =
            rows * tileSize;

        const float rangeSquared =
            interactionRange * interactionRange;

        // ----------------------------------
        // 1. Calculate current positions
        // ----------------------------------

        const SDL_FPoint actorCenter{
            actorHitbox.x + actorHitbox.w / 2.0f,
            actorHitbox.y + actorHitbox.h / 2.0f
        };

        const SDL_FPoint targetCenter{
            targetBounds.x + targetBounds.w / 2.0f,
            targetBounds.y + targetBounds.h / 2.0f
        };

        struct ApproachCandidate
        {
            SDL_FPoint position{};
            float distanceSquared = 0.0f;
        };

        std::vector<ApproachCandidate> candidates;

        // ----------------------------------
        // 2. Find valid positions near target
        // ----------------------------------

        for (int row = 0; row < rows; ++row)
        {
            for (int column = 0; column < columns; ++column)
            {
                // Center of the candidate tile.
                const SDL_FPoint position{
                    (static_cast<float>(column) + 0.5f) *
                        tileSize,
                    (static_cast<float>(row) + 0.5f) *
                        tileSize
                };

                // Match InteractionSystem's
                // center-to-center distance rule.
                const float targetDx =
                    position.x - targetCenter.x;

                const float targetDy =
                    position.y - targetCenter.y;

                const float distanceToTargetSquared =
                    targetDx * targetDx +
                    targetDy * targetDy;

                if (distanceToTargetSquared > rangeSquared)
                {
                    continue;
                }

                // Does the actor's entire hitbox fit here?
                const SDL_FRect candidateHitbox{
                    position.x - actorHitbox.w / 2.0f,
                    position.y - actorHitbox.h / 2.0f,
                    actorHitbox.w,
                    actorHitbox.h
                };

                const bool insideWorld =
                    candidateHitbox.x >= 0.0f &&
                    candidateHitbox.y >= 0.0f &&
                    candidateHitbox.x + candidateHitbox.w
                        <= worldWidth &&
                    candidateHitbox.y + candidateHitbox.h
                        <= worldHeight;

                if (!insideWorld ||
                    isBlocked(candidateHitbox))
                {
                    continue;
                }

                // Rank candidates by distance
                // from the actor's current position.
                const float actorDx =
                    position.x - actorCenter.x;

                const float actorDy =
                    position.y - actorCenter.y;

                candidates.push_back({
                    position,
                    actorDx * actorDx + actorDy * actorDy
                });
            }
        }

        // ----------------------------------
        // 3. Try closest candidates first
        // ----------------------------------

        std::sort(
            candidates.begin(),
            candidates.end(),
            [](const ApproachCandidate& a,
               const ApproachCandidate& b)
            {
                return a.distanceSquared <
                       b.distanceSquared;
            }
        );

        // ----------------------------------
        // 4. Find a reachable approach point
        // ----------------------------------

        for (const ApproachCandidate& candidate : candidates)
        {
            if (TryNavigateTo(
                    candidate.position,
                    actorHitbox,
                    columns,
                    rows,
                    tileSize,
                    isBlocked))
            {
                SDL_Log(
                    "Approach point selected: %.1f, %.1f",
                    candidate.position.x,
                    candidate.position.y
                );

                return true;
            }
        }

        SDL_Log("No reachable approach point found");

        return false;
    }


    bool NavigationController::TryNavigateNear(
        SDL_FPoint requestedDestination,
        const SDL_FRect& actorHitbox,
        int columns,
        int rows,
        float tileSize,
        const IsBlocked& isBlocked,
        float maxSnapDistance
    )
    {
        Cancel();
    
        if (!isBlocked ||
            columns <= 0 ||
            rows <= 0 ||
            tileSize <= 0.0f ||
            maxSnapDistance < 0.0f ||
            !std::isfinite(requestedDestination.x) ||
            !std::isfinite(requestedDestination.y))
        {
            return false;
        }
    
        const float worldWidth = columns * tileSize;
        const float worldHeight = rows * tileSize;
    
        // Reject clicks outside the map.
        if (requestedDestination.x < 0.0f ||
            requestedDestination.y < 0.0f ||
            requestedDestination.x >= worldWidth ||
            requestedDestination.y >= worldHeight)
        {
            return false;
        }
    
        // Reject clicks directly inside solid geometry.
        // A free pixel near a wall is different from
        // a click that actually lands inside the wall.
        const SDL_FRect clickedPoint{
            requestedDestination.x,
            requestedDestination.y,
            0.5f,
            0.5f
        };
    
        if (isBlocked(clickedPoint))
        {
            SDL_Log("Navigation: clicked solid geometry");
            return false;
        }
    
        // Always prefer the exact clicked destination.
        if (TryNavigateTo(
                requestedDestination,
                actorHitbox,
                columns,
                rows,
                tileSize,
                isBlocked))
        {
            return true;
        }
    
        // ----------------------------------
        // Search nearby destinations
        // ----------------------------------
    
        struct Candidate
        {
            SDL_FPoint position{};
            float distanceSquared = 0.0f;
        };
    
        std::vector<Candidate> candidates;
    
        constexpr float searchStep = 8.0f;
    
        const int steps =
            static_cast<int>(
                std::ceil(maxSnapDistance / searchStep)
            );
        
        const float maxDistanceSquared =
            maxSnapDistance * maxSnapDistance;
        
        for (int y = -steps; y <= steps; ++y)
        {
            for (int x = -steps; x <= steps; ++x)
            {
                const float dx = x * searchStep;
                const float dy = y * searchStep;
            
                const float distanceSquared =
                    dx * dx + dy * dy;
            
                if (distanceSquared == 0.0f ||
                    distanceSquared > maxDistanceSquared)
                {
                    continue;
                }
            
                candidates.push_back({
                    {
                        requestedDestination.x + dx,
                        requestedDestination.y + dy
                    },
                    distanceSquared
                });
            }
        }
    
        // Try the closest alternatives first.
        std::sort(
            candidates.begin(),
            candidates.end(),
            [](const Candidate& a, const Candidate& b)
            {
                return a.distanceSquared <
                       b.distanceSquared;
            }
        );
    
        for (const Candidate& candidate : candidates)
        {
            if (TryNavigateTo(
                    candidate.position,
                    actorHitbox,
                    columns,
                    rows,
                    tileSize,
                    isBlocked))
            {
                SDL_Log(
                    "Navigation: snapped destination "
                    "from (%.1f, %.1f) to (%.1f, %.1f)",
                    requestedDestination.x,
                    requestedDestination.y,
                    candidate.position.x,
                    candidate.position.y
                );
            
                return true;
            }
        }
    
        SDL_Log(
            "Navigation: no reachable position "
            "within %.0f pixels of click",
            maxSnapDistance
        );
    
        return false;
    }


}
