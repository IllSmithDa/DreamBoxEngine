
#include "DreamBox/WorldProgress.h"

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
        WorldProgress progress;

        const CellKey chestPosition{20, 13};
        const CellKey doorPosition{24, 3};

        // ----------------------------------
        // Test 1: New world has no progress
        // ----------------------------------

        Require(
            !progress.GetChest(1, chestPosition).has_value(),
            "New chest should have no recorded state"
        );

        std::cout << "PASS: Empty world state\n";

        // ----------------------------------
        // Test 2: Record opened chest
        // ----------------------------------

        progress.SetChest(
            1,
            chestPosition,
            ChestState{true, false}
        );

        const auto openedChest =
            progress.GetChest(1, chestPosition);

        Require(
            openedChest.has_value() &&
            openedChest->opened &&
            !openedChest->lootClaimed,
            "Opened chest state incorrect"
        );

        std::cout << "PASS: Chest opening\n";

        // ----------------------------------
        // Test 3: Record collected loot
        // ----------------------------------

        progress.SetChest(
            1,
            chestPosition,
            ChestState{true, true}
        );

        const auto claimedChest =
            progress.GetChest(1, chestPosition);

        Require(
            claimedChest.has_value() &&
            claimedChest->lootClaimed,
            "Loot collection state incorrect"
        );

        std::cout << "PASS: Loot collection\n";

        // ----------------------------------
        // Test 4: Different levels stay separate
        // ----------------------------------

        Require(
            !progress.GetChest(2, chestPosition).has_value(),
            "Level 2 inherited Level 1 state"
        );

        progress.SetChest(
            2,
            chestPosition,
            ChestState{true, false}
        );

        Require(
            !progress.GetChest(2, chestPosition)
                ->lootClaimed,
            "Level 2 chest should have unclaimed loot"
        );

        Require(
            progress.GetChest(1, chestPosition)
                ->lootClaimed,
            "Level 1 state was unexpectedly modified"
        );

        std::cout << "PASS: Independent levels\n";

        // ----------------------------------
        // Test 5: Door state
        // ----------------------------------

        progress.SetDoor(
            1,
            doorPosition,
            DoorState{true}
        );

        Require(
            progress.GetDoor(1, doorPosition)
                ->open,
            "Door state was not saved"
        );

        std::cout << "PASS: Door state\n";

        // ----------------------------------
        // Test 6: Clear progress
        // ----------------------------------

        progress.Clear();

        Require(
            !progress.GetChest(1, chestPosition).has_value(),
            "Chest state was not cleared"
        );

        Require(
            !progress.GetDoor(1, doorPosition).has_value(),
            "Door state was not cleared"
        );

        std::cout << "PASS: Clear progress\n";

        std::cout
            << "\nAll WorldProgress tests passed!\n";

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
