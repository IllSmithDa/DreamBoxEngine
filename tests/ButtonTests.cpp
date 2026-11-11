
#include "DreamBox/Button.h"

#include <iostream>

using namespace DreamBox;

int main()
{
    int failures = 0;

    auto Check = [&failures](
        bool condition,
        const char* testName
    )
    {
        if (condition)
        {
            std::cout << "[PASS] "
                      << testName << '\n';
        }
        else
        {
            std::cerr << "[FAIL] "
                      << testName << '\n';

            ++failures;
        }
    };

    // Same coordinates used by MainMenuScene.
    const SDL_FRect play{
        490.0f, 280.0f, 300.0f, 60.0f
    };

    const SDL_FRect settings{
        490.0f, 360.0f, 300.0f, 60.0f
    };

    const SDL_FRect quit{
        490.0f, 440.0f, 300.0f, 60.0f
    };

    // Test points inside buttons.
    Check(
        Button::ContainsPoint(play, 640.0f, 310.0f),
        "Play center is inside"
    );

    Check(
        Button::ContainsPoint(settings, 640.0f, 390.0f),
        "Settings center is inside"
    );

    Check(
        Button::ContainsPoint(quit, 640.0f, 470.0f),
        "Quit center is inside"
    );

    // Test points outside buttons.
    Check(
        !Button::ContainsPoint(play, 400.0f, 310.0f),
        "Point left of Play is outside"
    );

    Check(
        !Button::ContainsPoint(play, 800.0f, 310.0f),
        "Point right of Play is outside"
    );

    // Test rectangle boundaries.
    Check(
        Button::ContainsPoint(play, 490.0f, 280.0f),
        "Top-left boundary is inside"
    );

    Check(
        !Button::ContainsPoint(play, 790.0f, 310.0f),
        "Right boundary is outside"
    );

    Check(
        !Button::ContainsPoint(play, 640.0f, 340.0f),
        "Bottom boundary is outside"
    );

    // Check that buttons do not overlap logically.
    Check(
        !Button::ContainsPoint(settings, 640.0f, 310.0f),
        "Play center does not hit Settings"
    );

    Check(
        !Button::ContainsPoint(quit, 640.0f, 390.0f),
        "Settings center does not hit Quit"
    );

    // Edge case: zero-sized button.
    const SDL_FRect empty{
        100.0f, 100.0f, 0.0f, 0.0f
    };

    Check(
        !Button::ContainsPoint(empty, 100.0f, 100.0f),
        "Empty button cannot be hit"
    );

    if (failures == 0)
    {
        std::cout
            << "\nAll Button tests passed!\n";

        return 0;
    }

    std::cerr
        << "\nButton tests failed: "
        << failures << '\n';

    return 1;
}
