#include "DreamBox/GameScene.h"
#include "DreamBox/Collision.h"

namespace DreamBox
{
    GameScene::GameScene()
        : m_player{
            300.0f,
            200.0f,
            100.0f,
            100.0f,
            200.0f
        }
    {
        // Outer walls
        m_walls.emplace_back(
            50.0f,
            50.0f,
            40.0f,
            600.0f
        );

        m_walls.emplace_back(
            1190.0f,
            50.0f,
            40.0f,
            600.0f
        );

        m_walls.emplace_back(
            50.0f,
            50.0f,
            1180.0f,
            40.0f
        );

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
    }


    SceneRequest GameScene::Update(
        const Input& input,
        float deltaTime
    )
    {
        if (input.IsKeyPressed(SDL_SCANCODE_ESCAPE))
        {
            return SceneRequest::MainMenu;
        }

        SDL_FPoint movement =
            m_player.GetMovement(
                input,
                deltaTime
            );
        
        m_player.Move(
            movement.x,
            0.0f
        );
    
        if (PlayerCollidesWithWall())
        {
            m_player.Move(
                -movement.x,
                0.0f
            );
        }
    
        m_player.Move(
            0.0f,
            movement.y
        );
    
        if (PlayerCollidesWithWall())
        {
            m_player.Move(
                0.0f,
                -movement.y
            );
        }
    
        return SceneRequest::None;
    }


    void GameScene::Render(
        SDL_Renderer* renderer
    ) const
    {
        m_player.Render(renderer);

        for (const Wall& wall : m_walls)
        {
            wall.Render(renderer);
        }
    }


    bool GameScene::PlayerCollidesWithWall() const
    {
        for (const Wall& wall : m_walls)
        {
            if (Collision::Intersects(
                    m_player,
                    wall))
            {
                return true;
            }
        }

        return false;
    }
}