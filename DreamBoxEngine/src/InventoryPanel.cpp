
#include "DreamBox/InventoryPanel.h"
#include "DreamBox/Item.h"

#include <algorithm>
#include <string>

namespace DreamBox
{
    InventoryPanel::InventoryPanel(
        SDL_Renderer* renderer,
        const std::string& fontPath
    )
        : m_renderer(renderer),
          m_fontPath(fontPath)
    {
        m_title = std::make_unique<Text>(
            renderer,
            m_fontPath,
            30.0f,
            "INVENTORY"
        );

        m_footer = std::make_unique<Text>(
            renderer,
            m_fontPath,
            16.0f,
            "Press I or ESC to close"
        );

        m_slots.resize(VisibleSlots);
    }

    void InventoryPanel::Refresh(
        const Inventory& inventory
    )
    {
        const auto& stacks = inventory.GetStacks();

        // Recreate the slot labels from current data.
        m_slots.clear();
        m_slots.resize(VisibleSlots);

        const std::string slotCount =
            std::to_string(stacks.size()) +
            " / " +
            std::to_string(inventory.GetMaxSlots()) +
            " slots";

        m_slotCountText = std::make_unique<Text>(
            m_renderer,
            m_fontPath,
            17.0f,
            slotCount
        );

        const std::size_t count = std::min(
            stacks.size(),
            VisibleSlots
        );

        for (std::size_t i = 0; i < count; ++i)
        {
            const ItemStack& stack = stacks[i];

            m_slots[i].name = std::make_unique<Text>(
                m_renderer,
                m_fontPath,
                16.0f,
                GetItemName(stack.id)
            );

            m_slots[i].quantity = std::make_unique<Text>(
                m_renderer,
                m_fontPath,
                18.0f,
                "x" + std::to_string(stack.quantity)
            );
        }
    }

    void InventoryPanel::Render(
        SDL_Renderer* renderer
    ) const
    {
        if (!renderer)
        {
            return;
        }

        SDL_BlendMode previousBlendMode =
            SDL_BLENDMODE_NONE;

        SDL_GetRenderDrawBlendMode(
            renderer,
            &previousBlendMode
        );

        SDL_SetRenderDrawBlendMode(
            renderer,
            SDL_BLENDMODE_BLEND
        );

        // ----------------------------------
        // Darken the gameplay background
        // ----------------------------------

        const SDL_FRect backdrop{
            0.0f, 0.0f, 1280.0f, 720.0f
        };

        SDL_SetRenderDrawColor(
            renderer,
            0, 0, 0, 170
        );

        SDL_RenderFillRect(renderer, &backdrop);

        // ----------------------------------
        // Main inventory panel
        // ----------------------------------

        const SDL_FRect panel{
            268.0f, 83.0f, 744.0f, 553.0f
        };

        SDL_SetRenderDrawColor(
            renderer,
            28, 40, 56, 245
        );
        SDL_RenderFillRect(renderer, &panel);

        SDL_SetRenderDrawColor(
            renderer,
            110, 140, 180, 255
        );
        SDL_RenderRect(renderer, &panel);

        if (m_title)
        {
            m_title->RenderCentered(
                renderer,
                640.0f,
                112.0f
            );
        }

        if (m_slotCountText)
        {
            m_slotCountText->RenderCentered(
                renderer,
                640.0f,
                155.0f
            );
        }

        // ----------------------------------
        // Draw the 12 inventory slots
        // ----------------------------------

        constexpr int columns = 4;

        constexpr float startX = 308.0f;
        constexpr float startY = 196.0f;

        constexpr float slotWidth = 158.0f;
        constexpr float slotHeight = 106.0f;

        constexpr float spacingX = 12.0f;
        constexpr float spacingY = 12.0f;

        for (std::size_t i = 0;
             i < VisibleSlots;
             ++i)
        {
            const int column =
                static_cast<int>(i) % columns;

            const int row =
                static_cast<int>(i) / columns;

            const SDL_FRect slot{
                startX +
                    column * (slotWidth + spacingX),

                startY +
                    row * (slotHeight + spacingY),

                slotWidth,
                slotHeight
            };

            SDL_SetRenderDrawColor(
                renderer,
                42, 55, 73, 255
            );

            SDL_RenderFillRect(renderer, &slot);

            SDL_SetRenderDrawColor(
                renderer,
                90, 110, 135, 255
            );

            SDL_RenderRect(renderer, &slot);

            const float centerX =
                slot.x + slot.w / 2.0f;

            // Draw item name.
            if (m_slots[i].name)
            {
                m_slots[i].name->RenderCentered(
                    renderer,
                    centerX,
                    slot.y + 17.0f
                );
            }

            // Draw item quantity.
            if (m_slots[i].quantity)
            {
                m_slots[i].quantity->RenderCentered(
                    renderer,
                    centerX,
                    slot.y + 62.0f
                );
            }
        }

        // ----------------------------------
        // Footer
        // ----------------------------------

        if (m_footer)
        {
            m_footer->RenderCentered(
                renderer,
                640.0f,
                586.0f
            );
        }

        // Restore the renderer's prior setting.
        SDL_SetRenderDrawBlendMode(
            renderer,
            previousBlendMode
        );
    }
}
