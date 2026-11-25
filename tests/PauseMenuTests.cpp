
#include "DreamBox/PauseMenuModel.h"

#include <iostream>
#include <stdexcept>

using namespace DreamBox;

void Require(
    bool condition,
    const char* message
)
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
        PauseMenuModel menu;

        // ----------------------------------
        // Test 1: Default selection
        // ----------------------------------

        Require(
            menu.GetSelected() ==
                PauseMenuOption::Resume,
            "Default selection should be Resume"
        );

        std::cout << "PASS: Default selection\n";

        // ----------------------------------
        // Test 2: Move down
        // ----------------------------------

        menu.MoveSelection(1);

        Require(
            menu.GetSelected() ==
                PauseMenuOption::MainMenu,
            "Moving down should select Main Menu"
        );

        std::cout << "PASS: Down navigation\n";

        // ----------------------------------
        // Test 3: Wrap to first option
        // ----------------------------------

        menu.MoveSelection(1);

        Require(
            menu.GetSelected() ==
                PauseMenuOption::Resume,
            "Down navigation should wrap"
        );

        std::cout << "PASS: Down wrapping\n";

        // ----------------------------------
        // Test 4: Wrap upward
        // ----------------------------------

        menu.MoveSelection(-1);

        Require(
            menu.GetSelected() ==
                PauseMenuOption::MainMenu,
            "Up navigation should wrap"
        );

        std::cout << "PASS: Up wrapping\n";

        // ----------------------------------
        // Test 5: Direct selection
        // ----------------------------------

        menu.Select(PauseMenuOption::Resume);

        Require(
            menu.GetSelected() ==
                PauseMenuOption::Resume,
            "Direct selection failed"
        );

        std::cout << "PASS: Direct selection\n";

        // ----------------------------------
        // Test 6: Reset
        // ----------------------------------

        menu.Select(PauseMenuOption::MainMenu);
        menu.Reset();

        Require(
            menu.GetSelected() ==
                PauseMenuOption::Resume,
            "Reset should select Resume"
        );

        std::cout << "PASS: Reset selection\n";

        std::cout
            << "\nAll PauseMenu tests passed!\n";

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
