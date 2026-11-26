
#include "DreamBox/Pathfinder.h"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <limits>
#include <queue>

namespace DreamBox
{
    std::vector<GridPoint> Pathfinder::FindPath(
        GridPoint start,
        GridPoint goal,
        int columns,
        int rows,
        const IsWalkable& isWalkable
    )
    {
        if (columns <= 0 || rows <= 0 || !isWalkable)
        {
            return {};
        }

        const auto inBounds = [=](GridPoint p)
        {
            return p.x >= 0 &&
                   p.y >= 0 &&
                   p.x < columns &&
                   p.y < rows;
        };

        if (!inBounds(start) || !inBounds(goal) ||
            !isWalkable(start.x, start.y) ||
            !isWalkable(goal.x, goal.y))
        {
            return {};
        }

        const auto toIndex = [columns](GridPoint p)
        {
            return p.y * columns + p.x;
        };

        const auto fromIndex = [columns](int index)
        {
            return GridPoint{
                index % columns,
                index / columns
            };
        };

        // Manhattan distance: suitable for
        // movement in four cardinal directions.
        const auto heuristic = [goal](GridPoint p)
        {
            return std::abs(goal.x - p.x) +
                   std::abs(goal.y - p.y);
        };

        struct Entry
        {
            int index;
            int cost;
        };

        struct Greater
        {
            bool operator()(
                const Entry& first,
                const Entry& second
            ) const
            {
                return first.cost > second.cost;
            }
        };

        const int cellCount = columns * rows;

        std::vector<int> gScore(
            cellCount,
            std::numeric_limits<int>::max()
        );

        std::vector<int> parent(cellCount, -1);
        std::vector<bool> closed(cellCount, false);

        std::priority_queue<
            Entry,
            std::vector<Entry>,
            Greater
        > open;

        const int startIndex = toIndex(start);
        const int goalIndex = toIndex(goal);

        gScore[startIndex] = 0;

        open.push({
            startIndex,
            heuristic(start)
        });

        constexpr std::array<GridPoint, 4> directions{{
            { 1,  0 },
            {-1,  0 },
            { 0,  1 },
            { 0, -1 }
        }};

        while (!open.empty())
        {
            const int currentIndex =
                open.top().index;

            open.pop();

            if (closed[currentIndex])
            {
                continue;
            }

            closed[currentIndex] = true;

            // Destination reached.
            if (currentIndex == goalIndex)
            {
                std::vector<GridPoint> path;

                for (int index = goalIndex;
                     index != -1;
                     index = parent[index])
                {
                    path.push_back(
                        fromIndex(index)
                    );
                }

                std::reverse(
                    path.begin(),
                    path.end()
                );

                return path;
            }

            const GridPoint current =
                fromIndex(currentIndex);

            for (const GridPoint& direction : directions)
            {
                const GridPoint next{
                    current.x + direction.x,
                    current.y + direction.y
                };

                if (!inBounds(next) ||
                    !isWalkable(next.x, next.y))
                {
                    continue;
                }

                const int nextIndex = toIndex(next);

                if (closed[nextIndex])
                {
                    continue;
                }

                const int tentativeCost =
                    gScore[currentIndex] + 1;

                if (tentativeCost < gScore[nextIndex])
                {
                    parent[nextIndex] = currentIndex;
                    gScore[nextIndex] = tentativeCost;

                    open.push({
                        nextIndex,
                        tentativeCost + heuristic(next)
                    });
                }
            }
        }

        // No reachable route.
        return {};
    }
}