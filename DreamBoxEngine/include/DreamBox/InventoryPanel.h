
#pragma once

#include "DreamBox/Inventory.h"
#include "DreamBox/Text.h"

#include <SDL3/SDL_render.h>

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace DreamBox
{
    class InventoryPanel
    {
    public:
        InventoryPanel(
            SDL_Renderer* renderer,
            const std::string& fontPath
        );

        // Rebuild displayed text from inventory data.
        void Refresh(const Inventory& inventory);

        void Render(SDL_Renderer* renderer) const;

    private:
        struct SlotLabel
        {
            std::unique_ptr<Text> name;
            std::unique_ptr<Text> quantity;
        };

        SDL_Renderer* m_renderer = nullptr;
        std::string m_fontPath;

        std::unique_ptr<Text> m_title;
        std::unique_ptr<Text> m_slotCountText;
        std::unique_ptr<Text> m_footer;

        std::vector<SlotLabel> m_slots;

        static constexpr std::size_t VisibleSlots = 12;
    };
}
