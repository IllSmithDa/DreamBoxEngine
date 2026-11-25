
#pragma once

#include <map>
#include <optional>

namespace DreamBox
{
    // Identifies an object's position on the map.
    struct CellKey
    {
        int column = 0;
        int row = 0;

        auto operator<=>(const CellKey&) const = default;
    };

    struct ChestState
    {
        bool opened = false;
        bool lootClaimed = false;
    };

    struct DoorState
    {
        bool open = false;
    };

    class WorldProgress
    {
    public:
        // ----------------------------------
        // Chest state
        // ----------------------------------

        void SetChest(
            int level,
            CellKey position,
            ChestState state
        )
        {
            // Claimed loot implies the chest was opened.
            if (state.lootClaimed)
            {
                state.opened = true;
            }

            m_levels[level].chests[position] = state;
        }

        std::optional<ChestState> GetChest(
            int level,
            CellKey position
        ) const
        {
            const auto levelIt = m_levels.find(level);

            if (levelIt == m_levels.end())
            {
                return std::nullopt;
            }

            const auto chestIt =
                levelIt->second.chests.find(position);

            if (chestIt == levelIt->second.chests.end())
            {
                return std::nullopt;
            }

            return chestIt->second;
        }

        // ----------------------------------
        // Door state
        // ----------------------------------

        void SetDoor(
            int level,
            CellKey position,
            DoorState state
        )
        {
            m_levels[level].doors[position] = state;
        }

        std::optional<DoorState> GetDoor(
            int level,
            CellKey position
        ) const
        {
            const auto levelIt = m_levels.find(level);

            if (levelIt == m_levels.end())
            {
                return std::nullopt;
            }

            const auto doorIt =
                levelIt->second.doors.find(position);

            if (doorIt == levelIt->second.doors.end())
            {
                return std::nullopt;
            }

            return doorIt->second;
        }

        void Clear()
        {
            m_levels.clear();
        }

    private:
        struct LevelState
        {
            std::map<CellKey, ChestState> chests;
            std::map<CellKey, DoorState> doors;
        };

        std::map<int, LevelState> m_levels;
    };
}
