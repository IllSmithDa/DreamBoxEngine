#include "DreamBox/Engine.h"

namespace DreamBox
{
    Engine::~Engine()
    {
        Shutdown();
    }


    bool DreamBox::Engine::Initialize()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
            SDL_Log(
                "Failed to initialize SDL: %s",
                SDL_GetError()
            );

            return false;
        }

        if (!SDL_CreateWindowAndRenderer(
            "DreamBox Engine",
            1280,
            720,
            SDL_WINDOW_RESIZABLE,
            &m_window,
            &m_renderer))
        {
            SDL_Log(
                "Failed to create window and renderer: %s",
                SDL_GetError()
            );

            return false;
        }
        m_lastFrameTime = SDL_GetTicks();
        return true;
    }


    bool Engine::HandleEvent(const SDL_Event& event)
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            return false;
        }

        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.scancode == SDL_SCANCODE_ESCAPE)
            {
                return false;
            }
        }

        return true;
    }

    void Engine::Update()
    {
        // Calculate time since the previous frame.
        Uint64 currentTime = SDL_GetTicks();

        m_deltaTime =
            static_cast<float>(
                currentTime - m_lastFrameTime
                ) / 1000.0f;

        m_lastFrameTime = currentTime;

        SDL_FPoint movement =
            m_player.GetMovement(
                m_input,
                m_deltaTime
            );

        // Try horizontal movement first.
        m_player.Move(
            movement.x,
            0.0f
        );

        if (Collision::Intersects(
            m_player,
            m_wall))
        {
            // Undo only the horizontal movement.
            m_player.Move(
                -movement.x,
                0.0f
            );
        }

        // Then try vertical movement.
        m_player.Move(
            0.0f,
            movement.y
        );

        if (Collision::Intersects(
            m_player,
            m_wall))
        {
            // Undo only the vertical movement.
            m_player.Move(
                0.0f,
                -movement.y
            );
        }
    }

    void Engine::Render()
    {
        if (!m_renderer)
        {
            return;
        }

        SDL_SetRenderDrawColor(
            m_renderer,
            25,
            25,
            30,
            255
        );

        SDL_RenderClear(m_renderer);

        m_player.Render(m_renderer);
        m_wall.Render(m_renderer);

        SDL_RenderPresent(m_renderer);
    }


    void Engine::Shutdown()
    {
        if (m_renderer)
        {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }

        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Log("DreamBox Engine shut down.");
    }
}