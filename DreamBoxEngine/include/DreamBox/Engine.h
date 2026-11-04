#pragma once

#include <SDL3/SDL.h>
#include "DreamBox/Input.h"
#include "Player.h"
#include "DreamBox/Wall.h"
#include "DreamBox/Collision.h"


namespace DreamBox
{
    class Engine
    {
    public:
        Engine() = default;
        ~Engine();

        // Prevent accidentally copying the engine.
        Engine(const Engine&) = delete;
        Engine& operator=(const Engine&) = delete;

        bool Initialize();

        bool HandleEvent(const SDL_Event& event);

        void Update();
        void Render();

        void Shutdown();

    private:
    private:
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        Input m_input;

        Player m_player{
        300.0f,
        200.0f, 
        200.0f,
        150.0f,
        200.0f
        };


        std::vector<Wall> m_walls;

        Uint64 m_lastFrameTime = 0;
        float m_deltaTime = 0.0f;
    };
}