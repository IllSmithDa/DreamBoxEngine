#pragma once

#include <SDL3/SDL.h>
#include "DreamBox/Input.h"
#include "Player.h"
#include "DreamBox/Wall.h"
#include "DreamBox/Collision.h"
#include <vector>
#include <memory>
#include "DreamBox/Scene.h"
#include "DreamBox/GameScene.h"

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

        void ChangeScene(std::unique_ptr<Scene> scene);

    private:
    private:
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        Input m_input;
        std::unique_ptr<Scene> m_currentScene;
        Uint64 m_lastFrameTime = 0;
        float m_deltaTime = 0.0f;
    };
}