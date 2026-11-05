#include <memory>
#include "DreamBox/Engine.h"
#include "DreamBox/Scene.h"

namespace DreamBox
{
    Engine::~Engine()
    {
        Shutdown();
    }


    bool Engine::Initialize()
    {
        if (!SDL_Init(SDL_INIT_VIDEO))
        {
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
            return false;
        }

        m_lastFrameTime = SDL_GetTicks();

        // Left wall
        m_walls.emplace_back(
            50.0f,
            50.0f,
            40.0f,
            600.0f
        );

        // Right wall
        m_walls.emplace_back(
            1190.0f,
            50.0f,
            40.0f,
            600.0f
        );

        // Top wall
        m_walls.emplace_back(
            50.0f,
            50.0f,
            1180.0f,
            40.0f
        );

        // Bottom wall
        m_walls.emplace_back(
            50.0f,
            610.0f,
            1180.0f,
            40.0f
        );

        // Interior obstacle
        m_walls.emplace_back(
            600.0f,
            250.0f,
            60.0f,
            250.0f
        );

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
        Uint64 currentTime = SDL_GetTicks();

        m_deltaTime =
            static_cast<float>(
                currentTime - m_lastFrameTime
            ) / 1000.0f;

        m_lastFrameTime = currentTime;

        if (m_currentScene)
        {
            m_currentScene->Update(
                m_input,
                m_deltaTime
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

        if (m_currentScene)
        {
            m_currentScene->Render(
                m_renderer
            );
        }

        SDL_RenderPresent(m_renderer);
    }


    void Engine::Shutdown()
    {
        m_currentScene.reset();
    
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
    }
}