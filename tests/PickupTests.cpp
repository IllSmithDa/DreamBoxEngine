
#include "DreamBox/PickupSystem.h"

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
        PickupSystem pickups;
        Inventory inventory;

        const GroundPickup gold{
            {ItemId::GoldCoin, 5},
            {80.0f, 80.0f, 16.0f, 16.0f},
            false
        };

        pickups.SetPickups({gold});

        // ----------------------------------
        // Test 1: Distant items are not collected
        // ----------------------------------

        const SDL_FRect farPlayer{
            0.0f, 0.0f, 24.0f, 24.0f
        };

        Require(
            pickups.CollectNearby(
                farPlayer, inventory
            ).empty(),
            "Distant gold should not be collected"
        );

        std::cout << "PASS: Distance check\n";

        // ----------------------------------
        // Test 2: Clicking a pickup
        // ----------------------------------

        const auto destination =
            pickups.GetClickedDestination(
                {85.0f, 85.0f}
            );

        Require(
            destination.has_value(),
            "Clicking gold should return destination"
        );

        Require(
            destination->x == 88.0f &&
            destination->y == 88.0f,
            "Pickup destination should be its center"
        );

        std::cout << "PASS: Click targeting\n";

        // ----------------------------------
        // Test 3: Automatic collection
        // ----------------------------------

        // Player is six pixels left of gold.
        const SDL_FRect nearbyPlayer{
            50.0f, 80.0f, 24.0f, 24.0f
        };

        const auto collected =
            pickups.CollectNearby(
                nearbyPlayer, inventory
            );

        Require(
            collected.size() == 1,
            "Expected one collected item stack"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 5,
            "Inventory should contain five gold coins"
        );

        std::cout << "PASS: Nearby collection\n";

        // ----------------------------------
        // Test 4: No duplicate collection
        // ----------------------------------

        Require(
            pickups.CollectNearby(
                nearbyPlayer, inventory
            ).empty(),
            "Collected gold should not return"
        );

        Require(
            !pickups.GetClickedDestination(
                {85.0f, 85.0f}
            ).has_value(),
            "Collected gold should not be clickable"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 5,
            "Gold was collected more than once"
        );

        std::cout << "PASS: Duplicate prevention\n";

        // ----------------------------------
        // Test 5: Full inventory
        // ----------------------------------

        Inventory fullInventory(1, 99);

        Require(
            fullInventory.Add(ItemId::BronzeKey, 99),
            "Could not fill inventory"
        );

        PickupSystem blockedPickups;
        blockedPickups.SetPickups({gold});

        Require(
            blockedPickups.CollectNearby(
                nearbyPlayer, fullInventory
            ).empty(),
            "Full inventory must reject gold"
        );

        Require(
            !blockedPickups.GetPickups()[0].collected,
            "Failed collection removed ground item"
        );

        std::cout << "PASS: Full inventory\n";

        // ----------------------------------
        // Test 6: Retry after clearing space
        // ----------------------------------

        Require(
            fullInventory.Remove(
                ItemId::BronzeKey, 99
            ),
            "Could not clear inventory"
        );

        Require(
            blockedPickups.CollectNearby(
                nearbyPlayer, fullInventory
            ).size() == 1,
            "Gold should be collected after space opens"
        );

        Require(
            fullInventory.Count(ItemId::GoldCoin) == 5,
            "Recovered gold count is incorrect"
        );

        std::cout << "PASS: Retry collection\n";

        std::cout
            << "\nAll PickupTests passed!\n";

        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: "
                  << error.what() << '\n';

        return 1;
    }
}
