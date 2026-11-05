#pragma once

#include <vector>

#include "DreamBox/Scene.h"
#include "DreamBox/Player.h"
#include "DreamBox/Wall.h"

namespace DreamBox
{
    class GameScene : public Scene
    {
    public:
        GameScene();

        void Update(
            const Input& input,
            float deltaTime
        ) override;

        void Render(
            SDL_Renderer* renderer
        ) const override;

    private:
        bool PlayerCollidesWithWall() const;

        Player m_player;

        std::vector<Wall> m_walls;
    };
}