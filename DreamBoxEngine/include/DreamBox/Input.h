#pragma once

#include <SDL3/SDL.h>
#include <array>

namespace DreamBox
{
    class Input
    {
    public:
        Input() = default;

        void Update();

        bool IsKeyDown(SDL_Scancode key) const;
        bool IsKeyPressed(SDL_Scancode key) const;

        // mouse inputs 

        void HandleEvent(const SDL_Event& event);

        float GetMouseX() const;
        float GetMouseY() const;
            
        bool WasMouseMoved() const;
        bool WasLeftMousePressed() const;
            
        float GetClickX() const;
        float GetClickY() const;

    private:
        std::array<bool, SDL_SCANCODE_COUNT> m_currentKeys{};
        std::array<bool, SDL_SCANCODE_COUNT> m_previousKeys{};

        // mouse variables
        float m_mouseX = 0.0f;
        float m_mouseY = 0.0f;

        float m_clickX = 0.0f;
        float m_clickY = 0.0f;

        bool m_pendingMouseMoved = false;
        bool m_pendingLeftClick = false;

        bool m_mouseMoved = false;
        bool m_leftMousePressed = false;
    };
}