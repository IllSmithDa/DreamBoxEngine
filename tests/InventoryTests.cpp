
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
        Inventory inventory(3, 5);

        // Test 1: Adding items.
        Require(
            inventory.Add(ItemId::GoldCoin, 3),
            "Could not add gold coins"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 3,
            "Gold count incorrect"
        );

        std::cout << "PASS: Adding items\n";

        // Test 2: Stack splitting.
        Require(
            inventory.Add(ItemId::GoldCoin, 9),
            "Could not add more coins"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 12,
            "Total stacked quantity incorrect"
        );

        Require(
            inventory.GetStacks().size() == 3,
            "Expected three stacks"
        );

        std::cout << "PASS: Stack splitting\n";

        // Test 3: Full inventory remains unchanged.
        Require(
            !inventory.Add(ItemId::HealthPotion, 1),
            "Full inventory should reject new item"
        );

        Require(
            inventory.Count(ItemId::HealthPotion) == 0,
            "Failed addition changed inventory"
        );

        Require(
            !inventory.Add(ItemId::GoldCoin, 4),
            "Oversized addition should fail"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 12,
            "Failed addition changed gold quantity"
        );

        std::cout << "PASS: Inventory capacity\n";

        // Test 4: Removing across stacks.
        Require(
            inventory.Remove(ItemId::GoldCoin, 8),
            "Could not remove coins"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 4,
            "Removal count incorrect"
        );

        std::cout << "PASS: Item removal\n";

        // Test 5: Reject invalid operations.
        Require(
            !inventory.Remove(ItemId::GoldCoin, 10),
            "Should not remove unavailable items"
        );

        Require(
            !inventory.Add(ItemId::GoldCoin, 0),
            "Zero-quantity additions should fail"
        );

        Require(
            inventory.Count(ItemId::GoldCoin) == 4,
            "Invalid operations modified inventory"
        );

        std::cout << "PASS: Invalid operations\n";

        std::cout
            << "\nAll Inventory tests passed!\n";

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
