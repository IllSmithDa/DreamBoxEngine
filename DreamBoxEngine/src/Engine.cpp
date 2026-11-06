#include <memory>
#include "DreamBox/Engine.h"
#include "DreamBox/Scene.h"
#include "DreamBox/MainMenuScene.h"

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

        ChangeScene(
            std::make_unique<MainMenuScene>()
        );

        return true;
    }


    bool Engine::HandleEvent(const SDL_Event& event)
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            return false;
        }

        return true;
    }

    bool Engine::Update()
    {
        Uint64 currentTime =
            SDL_GetTicks();
    
        m_deltaTime =
            static_cast<float>(
                currentTime - m_lastFrameTime
            ) / 1000.0f;
        
        m_lastFrameTime =
            currentTime;
        
        m_input.Update();
        
        if (!m_currentScene)
        {
            return true;
        }
    
        SceneRequest request =
            m_currentScene->Update(
                m_input,
                m_deltaTime
            );
        
        switch (request)
        {

        case SceneRequest::MainMenu:
            ChangeScene(
                std::make_unique<MainMenuScene>()
            );
            break;
        case SceneRequest::StartGame:
            ChangeScene(
                std::make_unique<GameScene>()
            );
            break;
        
        case SceneRequest::Quit:
            return false;
        
        case SceneRequest::None:
        default:
            break;
        }
    
        return true;
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

    void Engine::ChangeScene(std::unique_ptr<Scene> scene)
    {
        m_currentScene = std::move(scene);
    }
}