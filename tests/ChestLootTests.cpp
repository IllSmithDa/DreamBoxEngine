
#include "DreamBox/InteractionSystem.h"
#include "DreamBox/Inventory.h"

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
        const SDL_FRect chestBounds{
            120.0f, 0.0f,
            40.0f, 40.0f
        };

        const SDL_FRect nearPlayer{
            50.0f, 4.0f,
            48.0f, 32.0f
        };

        const SDL_FPoint click{
            140.0f, 20.0f
        };

        // ----------------------------------
        // Test 1: Collect reward by mouse
        // ----------------------------------

        InteractionSystem interactions;
        Inventory inventory;

        interactions.Initialize({chestBounds}, {});

        Require(
            interactions.HandlePointerPress(
                click, nearPlayer
            ),
            "Mouse should select chest"
        );

        Require(
            interactions.TryInteractSelected(nearPlayer),
            "Nearby mouse interaction should open chest"
        );

        Require(
            interactions.CollectChestLoot(inventory) == 1,
            "Expected one collected chest reward"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 25,
            "Chest should award 25 gold coins"
        );

        std::cout << "PASS: Mouse chest collection\n";

        // ----------------------------------
        // Test 2: Prevent duplicate rewards
        // ----------------------------------

        Require(
            interactions.CollectChestLoot(inventory) == 0,
            "Chest reward should not be collected twice"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 25,
            "Inventory should still contain 25 coins"
        );

        std::cout << "PASS: Duplicate prevention\n";

        // ----------------------------------
        // Test 3: Full inventory
        // ----------------------------------

        InteractionSystem fullInteractions;

        // Only one slot, already full of keys.
        Inventory fullInventory(1, 99);

        Require(
            fullInventory.Add(ItemId::BronzeKey, 99),
            "Could not fill test inventory"
        );

        fullInteractions.Initialize(
            {chestBounds}, {}
        );

        fullInteractions.HandlePointerPress(
            click, nearPlayer
        );

        Require(
            fullInteractions.TryInteractSelected(nearPlayer),
            "Chest should open"
        );

        Require(
            fullInteractions.CollectChestLoot(
                fullInventory
            ) == 0,
            "Full inventory must reject reward"
        );

        Require(
            fullInventory.Count(ItemId::GoldCoin) == 0,
            "Rejected reward must not enter inventory"
        );

        std::cout << "PASS: Full inventory protection\n";

        // ----------------------------------
        // Test 4: Recover unclaimed reward
        // ----------------------------------

        Require(
            fullInventory.Remove(
                ItemId::BronzeKey, 99
            ),
            "Could not clear inventory"
        );

        Require(
            fullInteractions.CollectChestLoot(
                fullInventory
            ) == 1,
            "Unclaimed reward should be recoverable"
        );

        Require(
            fullInventory.Count(ItemId::GoldCoin) == 25,
            "Recovered loot count is incorrect"
        );

        std::cout << "PASS: Reward recovery\n";

        // ----------------------------------
        // Test 5: Keyboard interaction
        // ----------------------------------

        InteractionSystem keyboardInteractions;
        Inventory keyboardInventory;

        keyboardInteractions.Initialize(
            {chestBounds}, {}
        );

        keyboardInteractions.Update(
            true, nearPlayer
        );

        Require(
            keyboardInteractions.CollectChestLoot(
                keyboardInventory
            ) == 1,
            "Keyboard-opened chest should award loot"
        );

        Require(
            keyboardInventory.Count(
                ItemId::GoldCoin
            ) == 25,
            "Keyboard reward count is incorrect"
        );

        std::cout << "PASS: Keyboard chest collection\n";

        std::cout
            << "\nAll chest loot tests passed!\n";

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
