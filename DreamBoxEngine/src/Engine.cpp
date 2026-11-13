#include <memory>
#include <SDL3_ttf/SDL_ttf.h>
#include "DreamBox/Engine.h"
#include "DreamBox/Scene.h"
#include "DreamBox/MainMenuScene.h"
#include "DreamBox/GameScene.h"
#include "DreamBox/SettingsScene.h"

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

        if (!TTF_Init())
        {
            SDL_Log(
                "Failed to initialize SDL_ttf: %s",
                SDL_GetError()
            );
        
            return false;
        }

        if (!SDL_SetRenderLogicalPresentation(
            m_renderer,
            1280,
            720,
            SDL_LOGICAL_PRESENTATION_LETTERBOX
        ))
        {
            SDL_Log(
                "Failed to set logical presentation: %s",
                SDL_GetError()
            );
        }

        m_settings.fullscreen =
            (SDL_GetWindowFlags(m_window) &
             SDL_WINDOW_FULLSCREEN) != 0;

        int currentVSync = 0;

        if (SDL_GetRenderVSync(m_renderer, &currentVSync))
        {
            m_settings.vsync = currentVSync != 0;
        }
        else
        {
            SDL_Log(
                "Unable to read VSync state: %s",
                SDL_GetError()
            );
        }

        m_lastFrameTime = SDL_GetTicksNS();

        ChangeScene(
            std::make_unique<MainMenuScene>(m_renderer)
        );

        return true;
    }



    bool Engine::HandleEvent(const SDL_Event& event)
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            return false;
        }
    
        SDL_Event convertedEvent = event;
    
        if (m_renderer)
        {
            if (!SDL_ConvertEventToRenderCoordinates(
                m_renderer,
                &convertedEvent
            ))
            {
                SDL_Log(
                    "Failed to convert input coordinates: %s",
                    SDL_GetError()
                );
            
                return true;
            }
        }
    
        m_input.HandleEvent(convertedEvent);
    
        return true;
    }


    bool Engine::Update()
    {
        const Uint64 currentTime = SDL_GetTicksNS();
            
        m_deltaTime = static_cast<float>(
            static_cast<double>(
                currentTime - m_lastFrameTime
            ) / 1'000'000'000.0
        );

        
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
                std::make_unique<MainMenuScene>(m_renderer)
            );
            break;
        case SceneRequest::StartGame:
            ChangeScene(
                std::make_unique<GameScene>(
                    m_renderer
                )
            );
            break;
        case SceneRequest::Settings:
            OpenSettings();
            break;
        case SceneRequest::ToggleFullscreen:
            ToggleFullscreen();
            OpenSettings(0);
            break;

        case SceneRequest::ToggleVSync:
            ToggleVSync();
            OpenSettings(1);
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

        TTF_Quit();
        
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
        SDL_Quit();
    }

    void Engine::ChangeScene(std::unique_ptr<Scene> scene)
    {
        m_currentScene = std::move(scene);
    }

    void Engine::OpenSettings(int selectedIndex)
    {
        ChangeScene(
            std::make_unique<SettingsScene>(
                m_renderer,
                m_settings,
                selectedIndex
            )
        );
    }

    void Engine::ToggleFullscreen()
    {
        if (!m_window)
        {
            return;
        }

        bool requestedFullscreen =
            !m_settings.fullscreen;

        if (!SDL_SetWindowFullscreen(
            m_window,
            requestedFullscreen
        ))
        {
            SDL_Log(
                "Failed to toggle fullscreen: %s",
                SDL_GetError()
            );
            return;
        }

        m_settings.fullscreen = requestedFullscreen;
    }

    void Engine::ToggleVSync()
    {
        if (!m_renderer)
        {
            return;
        }

        bool requestedVSync =
            !m_settings.vsync;

        int interval =
            requestedVSync ? 1 : 0;

        if (!SDL_SetRenderVSync(
            m_renderer,
            interval
        ))
        {
            SDL_Log(
                "Failed to toggle VSync: %s",
                SDL_GetError()
            );
            return;
        }

        int actualVSync = 0;

        if (SDL_GetRenderVSync(
            m_renderer,
            &actualVSync
        ))
        {
            m_settings.vsync = actualVSync != 0;
        }
        else
        {
            m_settings.vsync = requestedVSync;
        }
    }
}