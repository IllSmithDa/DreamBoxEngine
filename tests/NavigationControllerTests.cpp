
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
