#pragma once

#include <vector>
#include <memory>
#include "DreamBox/Scene.h"
#include "DreamBox/Player.h"
#include "DreamBox/Wall.h"
#include "DreamBox/Texture.h"
namespace DreamBox
{
    class GameScene : public Scene
    {
    public:
        GameScene(SDL_Renderer* renderer);

        SceneRequest Update(
            const Input& input,
            float deltaTime
        ) override;

        void Render(
            SDL_Renderer* renderer
        ) const override;

    private:
        bool PlayerCollidesWithWall() const;
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        Input m_input;
        std::unique_ptr<Scene> m_currentScene;
        Uint64 m_lastFrameTime = 0;
        float m_deltaTime = 0.0f;
        private:
        Player m_player;
        std::vector<Wall> m_walls;

        // textures 
        std::unique_ptr<Texture> m_playerTexture;
    };
}