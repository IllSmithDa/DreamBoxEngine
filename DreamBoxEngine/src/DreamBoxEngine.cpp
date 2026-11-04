#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "DreamBox/Engine.h"


// Called once when DreamBox Engine starts.
SDL_AppResult SDL_AppInit(
    void** appstate,
    int argc,
    char* argv[]
)
{
    auto* engine = new DreamBox::Engine();

    *appstate = engine;

    if (!engine->Initialize())
    {
        delete engine;
        *appstate = nullptr;

        return SDL_APP_FAILURE;
    }

    SDL_Log("DreamBox Engine started successfully.");

    return SDL_APP_CONTINUE;
}


// Called whenever SDL sends an event.
SDL_AppResult SDL_AppEvent(
    void* appstate,
    SDL_Event* event
)
{
    auto* engine =
        static_cast<DreamBox::Engine*>(appstate);

    if (!engine->HandleEvent(*event))
    {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}


// Called repeatedly while the application is running.
SDL_AppResult SDL_AppIterate(void* appstate)
{
    auto* engine =
        static_cast<DreamBox::Engine*>(appstate);

    engine->Update();
    engine->Render();

    return SDL_APP_CONTINUE;
}


// Called once when DreamBox Engine shuts down.
void SDL_AppQuit(
    void* appstate,
    SDL_AppResult result
)
{
    auto* engine =
        static_cast<DreamBox::Engine*>(appstate);

    delete engine;

    SDL_Log("DreamBox Engine application ended.");
}