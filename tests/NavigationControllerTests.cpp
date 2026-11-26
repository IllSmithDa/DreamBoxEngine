
#include "DreamBox/NavigationController.h"

#include <SDL3/SDL_rect.h>
#include <cmath>
#include <iostream>
#include <stdexcept>

using namespace DreamBox;

void Require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}


void TestApproachPointNavigation()
{
    using namespace DreamBox;

    NavigationController navigation;

    // Player's collision hitbox: 48 x 32.
    SDL_FRect player{
        36.0f, 124.0f,
        48.0f, 32.0f
    };

    // Solid chest-like target.
    const SDL_FRect target{
        280.0f, 120.0f,
        40.0f, 40.0f
    };

    const auto isBlocked =
        [&](const SDL_FRect& candidate)
    {
        return SDL_HasRectIntersectionFloat(
            &candidate,
            &target
        );
    };

    constexpr float range = 70.0f;

    // ----------------------------------
    // Test successful approach planning
    // ----------------------------------

    Require(
        navigation.TryNavigateWithinRange(
            target,
            player,
            range,
            12, 8, 40.0f,
            isBlocked
        ),
        "Expected a reachable approach point"
    );

    Require(
        navigation.HasPath(),
        "Approach should create an active route"
    );

    const auto destination =
        navigation.GetDestination();

    Require(
        destination.has_value(),
        "Approach destination should exist"
    );

    const float targetCenterX =
        target.x + target.w / 2.0f;

    const float targetCenterY =
        target.y + target.h / 2.0f;

    const float dx =
        destination->x - targetCenterX;

    const float dy =
        destination->y - targetCenterY;

    Require(
        dx * dx + dy * dy <= range * range,
        "Approach point is outside interaction range"
    );

    const SDL_FRect destinationHitbox{
        destination->x - player.w / 2.0f,
        destination->y - player.h / 2.0f,
        player.w,
        player.h
    };

    Require(
        !isBlocked(destinationHitbox),
        "Approach point overlaps the solid target"
    );

    std::cout << "PASS: Approach-point selection\n";

    // ----------------------------------
    // Test that movement reaches the target
    // ----------------------------------

    for (int frame = 0;
         frame < 500 && navigation.HasPath();
         ++frame)
    {
        const SDL_FPoint movement =
            navigation.CalculateMovement(
                player,
                0.05f
            );

        const SDL_FRect next{
            player.x + movement.x,
            player.y + movement.y,
            player.w,
            player.h
        };

        Require(
            !isBlocked(next),
            "Approach movement crossed solid target"
        );

        player = next;

        navigation.UpdateAfterMovement(
            movement,
            movement,
            player
        );
    }

    Require(
        !navigation.HasPath(),
        "Approach route should finish"
    );

    const float finalCenterX =
        player.x + player.w / 2.0f;

    const float finalCenterY =
        player.y + player.h / 2.0f;

    const float finalDx =
        finalCenterX - targetCenterX;

    const float finalDy =
        finalCenterY - targetCenterY;

    Require(
        finalDx * finalDx + finalDy * finalDy
            <= range * range,
        "Actor stopped outside interaction range"
    );

    std::cout << "PASS: Automatic approach movement\n";

    // ----------------------------------
    // Test unreachable approach points
    // ----------------------------------

    const auto alwaysBlocked =
        [](const SDL_FRect&)
    {
        return true;
    };

    Require(
        !navigation.TryNavigateWithinRange(
            target,
            player,
            range,
            12, 8, 40.0f,
            alwaysBlocked
        ),
        "Should reject an unreachable approach"
    );

    Require(
        !navigation.HasPath(),
        "Failed approach must not leave an active route"
    );

    std::cout << "PASS: Unreachable approach\n";
}


void TestNearWallDestination()
{
    NavigationController navigation;

    const SDL_FRect actor{
        36.0f, 124.0f, 48.0f, 32.0f
    };

    const SDL_FRect wall{
        160.0f, 0.0f, 40.0f, 320.0f
    };

    const auto isBlocked =
        [&](const SDL_FRect& candidate)
    {
        return SDL_HasRectIntersectionFloat(
            &candidate, &wall
        );
    };

    // This is a valid destination, but its
    // grid tile center (140, 140) is blocked.
    Require(
        navigation.TryNavigateTo(
            {135.0f, 140.0f},
            actor, 10, 8, 40.0f,
            isBlocked
        ),
        "Valid near-wall goal should be reachable"
    );

    navigation.Cancel();

    // This click is on free floor, but the
    // actor's hitbox cannot be centered here.
    const SDL_FPoint click{150.0f, 140.0f};

    Require(
        navigation.TryNavigateNear(
            click,
            actor, 10, 8, 40.0f,
            isBlocked,
            32.0f
        ),
        "Expected a nearby valid destination"
    );

    const auto destination =
        navigation.GetDestination();

    Require(
        destination.has_value(),
        "Snapped destination should exist"
    );

    const SDL_FRect finalHitbox{
        destination->x - actor.w / 2.0f,
        destination->y - actor.h / 2.0f,
        actor.w,
        actor.h
    };

    Require(
        !isBlocked(finalHitbox),
        "Snapped destination must not overlap wall"
    );

    const float dx = destination->x - click.x;
    const float dy = destination->y - click.y;

    Require(
        dx * dx + dy * dy <= 32.0f * 32.0f,
        "Snapped destination is too far from click"
    );

    // Clicking directly inside the wall
    // must still be rejected.
    Require(
        !navigation.TryNavigateNear(
            {170.0f, 140.0f},
            actor, 10, 8, 40.0f,
            isBlocked,
            32.0f
        ),
        "Click inside wall should be rejected"
    );

    std::cout << "PASS: Near-wall navigation\n";
}


int main()
{
    try
    {
        NavigationController navigation;

        // Start in tile (1, 2) of a 7x5 grid.
        SDL_FRect player{
            55.0f, 95.0f,
            10.0f, 10.0f
        };

        // Vertical wall blocks the direct route.
        const SDL_FRect wall{
            120.0f, 40.0f,
            40.0f, 120.0f
        };

        const auto isBlocked =
            [&](const SDL_FRect& candidate)
        {
            return SDL_HasRectIntersectionFloat(
                &candidate,
                &wall
            );
        };

        // Test 1: Plan route around the wall.
        Require(
            navigation.TryNavigateTo(
                {220.0f, 100.0f},
                player,
                7, 5, 40.0f,
                isBlocked
            ),
            "Expected a valid route around wall"
        );

        Require(
            navigation.HasPath(),
            "Controller should have an active route"
        );

        std::cout << "PASS: Route planning\n";

        // Test 2: Follow the waypoints.
        for (int frame = 0;
             frame < 400 && navigation.HasPath();
             ++frame)
        {
            const SDL_FPoint movement =
                navigation.CalculateMovement(
                    player,
                    0.05f
                );

            SDL_FRect next{
                player.x + movement.x,
                player.y + movement.y,
                player.w,
                player.h
            };

            Require(
                !isBlocked(next),
                "Navigation crossed a wall"
            );

            player = next;

            navigation.UpdateAfterMovement(
                movement,
                movement,
                player
            );
        }

        Require(
            !navigation.HasPath(),
            "Route should finish after arrival"
        );

        const float centerX =
            player.x + player.w / 2.0f;

        const float centerY =
            player.y + player.h / 2.0f;

        Require(
            std::fabs(centerX - 220.0f) <= 1.0f &&
            std::fabs(centerY - 100.0f) <= 1.0f,
            "Player did not reach destination"
        );

        std::cout << "PASS: Waypoint following\n";

        // Test 3: Reject blocked destinations.
        Require(
            !navigation.TryNavigateTo(
                {140.0f, 100.0f},
                player,
                7, 5, 40.0f,
                isBlocked
            ),
            "Blocked destination should be rejected"
        );

        Require(
            !navigation.HasPath(),
            "Failed route should not remain active"
        );

        std::cout << "PASS: Blocked destination\n";

        // Test 4: Manual cancellation.
        Require(
            navigation.TryNavigateTo(
                {180.0f, 180.0f},
                player,
                7, 5, 40.0f,
                isBlocked
            ),
            "Expected a valid route"
        );

        navigation.Cancel();

        Require(
            !navigation.HasPath(),
            "Cancellation should clear active route"
        );

        std::cout << "PASS: Cancellation\n";

        TestApproachPointNavigation();

        // precision navigation test
        TestNearWallDestination();

        std::cout
            << "\nAll NavigationController tests passed!\n";

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr
            << "FAIL: " << error.what()
            << '\n';

        return 1;
    }
}
