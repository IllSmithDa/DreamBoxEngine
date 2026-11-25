#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <vector>
#include <cstddef>
#include "DreamBox/Scene.h"
#include "DreamBox/Player.h"
#include "DreamBox/Wall.h"
#include "DreamBox/Texture.h"
#include "DreamBox/SpriteAnimator.h"
#include "DreamBox/Tilemap.h"
#include "DreamBox/InteractionSystem.h"
#include "DreamBox/NavigationController.h"
#include "DreamBox/Inventory.h"
#include "DreamBox/InventoryPanel.h"

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

        // player sprites animation
        SpriteAnimator m_playerAnimator{
            16.0f,
            16.0f,
            4,
            0.15f
        };

        bool m_showCollisionBounds = false;

        // tilemap
        Tilemap m_tilemap;

        int m_currentLevel = 1;

        // Prevent repeated transitions while standing on an exit.
        bool m_wasOnExit = false;

        bool LoadLevel(int level);

        InteractionSystem m_interactions;

        std::optional<SDL_FPoint> m_hoverPoint;

        bool TrySetMovePath(const SDL_FPoint& destination);

        void CancelMovePath();

        bool TryNavigateToSelectedObject();


        enum class PendingAction
        {
            None,
            ObjectInteraction,
            LevelExit
        };

        PendingAction m_pendingAction = PendingAction::None;

        void CancelAutomaticAction();

        void HandlePointerNavigation(
            const SDL_FPoint& point
        );

        // navigation
        NavigationController m_navigation;

        Inventory m_inventory;
        
        enum class OverlayMode
        {
            None,
            Inventory
        };

        OverlayMode m_overlayMode = OverlayMode::None;

        std::unique_ptr<InventoryPanel> m_inventoryPanel;
    };
}