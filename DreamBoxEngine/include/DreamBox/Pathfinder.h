
#pragma once

#include <functional>
#include <vector>

namespace DreamBox
{
    struct GridPoint
    {
        int x = 0;
        int y = 0;

        bool operator==(const GridPoint&) const = default;
    };

    class Pathfinder
    {
    public:
        using IsWalkable =
            std::function<bool(int column, int row)>;

        // Returns a route including start and goal.
        // Returns an empty vector if no route exists.
        static std::vector<GridPoint> FindPath(
            GridPoint start,
            GridPoint goal,
            int columns,
            int rows,
            const IsWalkable& isWalkable
        );
    };
}