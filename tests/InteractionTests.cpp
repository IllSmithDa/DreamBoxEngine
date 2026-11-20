
#include "DreamBox/Chest.h"
#include "DreamBox/Door.h"
#include "DreamBox/InteractionSystem.h"

#include <SDL3/SDL_rect.h>

#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace DreamBox;

// Use exceptions instead of assert() so tests also
// run correctly in Release builds.
void Require(bool condition, const char* message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

// ----------------------------------
// Test 1: Chest behavior
// ----------------------------------
void TestChest()
{
    Chest chest(SDL_FRect{40, 40, 40, 40});

    const SDL_FRect player{0, 0, 10, 10};

    Require(
        chest.CanInteract(),
        "A new chest should be interactable"
    );

    Require(
        chest.IsSolid(),
        "A closed chest should be solid"
    );

    chest.Interact(player);

    Require(
        !chest.CanInteract(),
        "Opened chest should not be interactable"
    );

    Require(
        chest.IsSolid(),
        "Opened chest should remain solid"
    );

    // Repeated interaction must not reopen the chest.
    chest.Interact(player);

    Require(
        !chest.CanInteract(),
        "Chest should remain opened"
    );
}

// ----------------------------------
// Test 2: Door opens and closes
// ----------------------------------
void TestDoorToggle()
{
    Door door(SDL_FRect{40, 0, 20, 20});

    const SDL_FRect player{0, 0, 10, 10};

    Require(
        door.IsSolid(),
        "Closed door should block movement"
    );

    door.Interact(player);

    Require(
        !door.IsSolid(),
        "Open door should allow movement"
    );

    door.Interact(player);

    Require(
        door.IsSolid(),
        "Closed door should block movement again"
    );
}

// ----------------------------------
// Test 3: Door cannot close on player
// ----------------------------------
void TestDoorSafety()
{
    Door door(SDL_FRect{40, 0, 20, 20});

    const SDL_FRect outside{0, 0, 10, 10};
    const SDL_FRect inside{45, 5, 10, 10};

    door.Interact(outside);

    Require(
        !door.IsSolid(),
        "Door should be open"
    );

    // Attempt to close while player occupies doorway.
    door.Interact(inside);

    Require(
        !door.IsSolid(),
        "Door must remain open while occupied"
    );

    door.Interact(outside);

    Require(
        door.IsSolid(),
        "Door should close after player leaves"
    );
}

// ----------------------------------
// Test 4: E-key press and release
// ----------------------------------
void TestInteractionKeyState()
{
    InteractionSystem system;

    const SDL_FRect door{40, 0, 20, 20};
    const SDL_FRect player{0, 0, 10, 10};

    system.Initialize({}, {door});

    Require(
        system.CollidesWithSolid(door),
        "Door should initially be solid"
    );

    // First press opens the door.
    system.Update(true, player);

    Require(
        !system.CollidesWithSolid(door),
        "First press should open door"
    );

    // Holding E should not close it.
    system.Update(true, player);

    Require(
        !system.CollidesWithSolid(door),
        "Held E should not toggle door repeatedly"
    );

    // Release, then press again.
    system.Update(false, player);
    system.Update(true, player);

    Require(
        system.CollidesWithSolid(door),
        "Second distinct press should close door"
    );
}

// ----------------------------------
// Test 5: Select nearest door
// ----------------------------------
void TestNearestDoor()
{
    InteractionSystem system;

    const SDL_FRect nearDoor{25, 0, 20, 20};
    const SDL_FRect farDoor{60, 0, 20, 20};
    const SDL_FRect player{0, 0, 10, 10};

    system.Initialize({}, {farDoor, nearDoor});

    system.Update(true, player);

    Require(
        !system.CollidesWithSolid(nearDoor),
        "Nearest door should open"
    );

    Require(
        system.CollidesWithSolid(farDoor),
        "Farther door should remain closed"
    );
}

// ----------------------------------
// Test 6: Door competes with chest
// ----------------------------------
void TestNearestObjectType()
{
    InteractionSystem system;

    const SDL_FRect chest{55, 0, 20, 20};
    const SDL_FRect door{25, 0, 20, 20};
    const SDL_FRect player{0, 0, 10, 10};

    system.Initialize({chest}, {door});

    system.Update(true, player);

    Require(
        !system.CollidesWithSolid(door),
        "Closer door should be selected over chest"
    );

    Require(
        system.CollidesWithSolid(chest),
        "Chest should remain solid"
    );
}

// ----------------------------------
// Test 7: Interaction range
// ----------------------------------
void TestInteractionRange()
{
    InteractionSystem system;

    const SDL_FRect door{200, 0, 20, 20};
    const SDL_FRect player{0, 0, 10, 10};

    system.Initialize({}, {door});

    system.Update(true, player);

    Require(
        system.CollidesWithSolid(door),
        "Distant door should remain closed"
    );
}

// ----------------------------------
// Test 8: Level initialization reset
// ----------------------------------
void TestLevelReset()
{
    InteractionSystem system;

    const SDL_FRect door{40, 0, 20, 20};
    const SDL_FRect player{0, 0, 10, 10};

    system.Initialize({}, {door});

    system.Update(true, player);

    Require(
        !system.CollidesWithSolid(door),
        "Door should open before reset"
    );

    // Simulate loading a level again.
    system.Initialize({}, {door});

    Require(
        system.CollidesWithSolid(door),
        "Level initialization should reset door"
    );
}

// ----------------------------------
// Test runner
// ----------------------------------
int main()
{
    struct TestCase
    {
        const char* name;
        void (*run)();
    };

    const std::vector<TestCase> tests{
        {"Chest behavior", TestChest},
        {"Door toggle", TestDoorToggle},
        {"Door safety", TestDoorSafety},
        {"E-key state", TestInteractionKeyState},
        {"Nearest door", TestNearestDoor},
        {"Nearest object type", TestNearestObjectType},
        {"Interaction range", TestInteractionRange},
        {"Level reset", TestLevelReset}
    };

    int failed = 0;

    for (const TestCase& test : tests)
    {
        try
        {
            test.run();
            std::cout << "PASS: " << test.name << '\n';
        }
        catch (const std::exception& error)
        {
            ++failed;

            std::cerr
                << "FAIL: " << test.name
                << " - " << error.what()
                << '\n';
        }
    }

    std::cout
        << "\nTests: " << tests.size()
        << ", Failed: " << failed
        << '\n';

    return failed == 0 ? 0 : 1;
}
