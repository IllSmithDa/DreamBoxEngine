#include "DreamBox/Input.h"
#include <SDL3/SDL_touch.h>

namespace DreamBox
{
    void Input::Update()
    {
        // Preserve previous keyboard state
        m_previousKeys = m_currentKeys;
    
        const bool* keyboardState =
            SDL_GetKeyboardState(nullptr);
    
        for (int i = 0; i < SDL_SCANCODE_COUNT; ++i)
        {
            m_currentKeys[i] = keyboardState[i];
        }
    

        // Transfer pending events to this frame.
        m_mouseMoved = m_pendingMouseMoved;
        m_leftMousePressed = m_pendingLeftClick;
        m_pointPressed = m_pendingPointPressed;

        // Clear pending events for the next frame.
        m_pendingMouseMoved = false;
        m_pendingLeftClick = false;
        m_pendingPointPressed = false;

    }


    bool Input::IsKeyDown(SDL_Scancode key) const
    {
        return m_currentKeys[key];
    }


    bool Input::IsKeyPressed(SDL_Scancode key) const
    {
        return m_currentKeys[key] &&
               !m_previousKeys[key];
    }


    void Input::HandleEvent(const SDL_Event& event)
    {
        if (event.type == SDL_EVENT_MOUSE_MOTION)
        {
            m_mouseX = event.motion.x;
            m_mouseY = event.motion.y;
        
            m_pendingMouseMoved = true;
        }
    
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
            event.button.button == SDL_BUTTON_LEFT)
        {
            m_mouseX = event.button.x;
            m_mouseY = event.button.y;
        
            m_clickX = event.button.x;
            m_clickY = event.button.y;
        
            m_pendingLeftClick = true;
        }

        // Left mouse click.
        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
            event.button.button == SDL_BUTTON_LEFT &&
            event.button.which != SDL_TOUCH_MOUSEID)
        {
            m_pendingPointPressed = true;
        
            m_pointPosition = SDL_FPoint{
                event.button.x,
                event.button.y
            };
        }

        // Finger tap.
        if (event.type == SDL_EVENT_FINGER_DOWN &&
            event.tfinger.touchID != SDL_MOUSE_TOUCHID)
        {
            m_pendingPointPressed = true;
        
            m_pointPosition = SDL_FPoint{
                event.tfinger.x,
                event.tfinger.y
            };
        }

    }
    
    float Input::GetMouseX() const
    {
        return m_mouseX;
    }
    
    float Input::GetMouseY() const
    {
        return m_mouseY;
    }
    
    bool Input::WasMouseMoved() const
    {
        return m_mouseMoved;
    }
    
    bool Input::WasLeftMousePressed() const
    {
        return m_leftMousePressed;
    }
    
    float Input::GetClickX() const
    {
        return m_clickX;
    }
    
    float Input::GetClickY() const
    {
        return m_clickY;
    }

}