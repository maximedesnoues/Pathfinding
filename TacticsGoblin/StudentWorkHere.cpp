#include "FlowFieldMap.h"
#include "PlayerController.h"
#include "Map.h"
#include "Game.h"
#include "Unit.h"
#include "EntityType.h"
#include "PointOfInterrest.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <memory>
#include <queue>
#include <vector>

namespace
{
    constexpr unsigned int BLOCKED = 0xFFFFFFFFu;
    constexpr std::uint64_t STRAIGHT = 1000;
    constexpr std::uint64_t DIAGONAL = 1414;
    constexpr std::uint64_t INF = std::numeric_limits<std::uint64_t>::max();

    constexpr int directionX[8] = { 0, 1, 0, -1, 1, 1, -1, -1 };
    constexpr int directionY[8] = { -1, 0, 1, 0, -1, 1, 1, -1 };

    struct Node
    {
        std::uint64_t cost;
        int x;
        int y;
    };

    struct CompareNode
    {
        bool operator()(const Node& firstNode, const Node& secondNode) const
        {
            return firstNode.cost > secondNode.cost;
        }
    };

    using PriorityQueue = std::priority_queue<Node, std::vector<Node>, CompareNode>;

    std::size_t Index(int x, int y, int width)
    {
        return static_cast<std::size_t>(y) * width + x;
    }

    bool CanMove(const std::shared_ptr<Map>& map, int x, int y, int nextX, int nextY)
    {
        if (map->GetTileCost(nextX, nextY) == BLOCKED)
        {
            return false;
        }

        if (x != nextX && y != nextY)
        {
            if (map->GetTileCost(nextX, y) == BLOCKED || map->GetTileCost(x, nextY) == BLOCKED)
            {
                return false;
            }
        }

        return true;
    }

    std::uint64_t MoveCost(const std::shared_ptr<Map>& map, int x, int y, bool diagonal)
    {
        return static_cast<std::uint64_t>(map->GetTileCost(x, y)) * (diagonal ? DIAGONAL : STRAIGHT);
    }

    std::uint64_t Heuristic(int x, int y, int targetX, int targetY, bool allowDiags)
    {
        const std::uint64_t dx = static_cast<std::uint64_t>(std::abs(x - targetX));
        const std::uint64_t dy = static_cast<std::uint64_t>(std::abs(y - targetY));

        if (!allowDiags)
        {
            return (dx + dy) * STRAIGHT;
        }

        const std::uint64_t diagonalSteps = std::min(dx, dy);

        return (std::max(dx, dy) - diagonalSteps) * STRAIGHT + diagonalSteps * DIAGONAL;
    }
}

std::shared_ptr<PointOfInterrest> PlayerController::FindCheapestVillageUsingDijkstra(std::shared_ptr<Unit> unit, bool allowDiags = false)
{
    if (!game || !unit)
    {
        return nullptr;
    }

    auto map = game->GetMap();

    if (!map)
    {
        return nullptr;
    }

    const int width = map->GetWidth();
    const int height = map->GetHeight();

    if (width <= 0 || height <= 0)
    {
        return nullptr;
    }

    const CVect start = unit->GetTile();
    const int startX = static_cast<int>(start.x);
    const int startY = static_cast<int>(start.y);

    if (map->GetTileCost(startX, startY) == BLOCKED)
    {
        return nullptr;
    }

    std::vector<std::uint64_t> distances(static_cast<std::size_t>(width) * height, INF);
    PriorityQueue openList;

    distances[Index(startX, startY, width)] = 0;
    openList.push({ 0, startX, startY });

    while (!openList.empty())
    {
        const Node currentNode = openList.top();
        openList.pop();

        if (currentNode.cost != distances[Index(currentNode.x, currentNode.y, width)])
        {
            continue;
        }

        for (int i = 0; i < (allowDiags ? 8 : 4); ++i)
        {
            const int nextX = currentNode.x + directionX[i];
            const int nextY = currentNode.y + directionY[i];

            if (!CanMove(map, currentNode.x, currentNode.y, nextX, nextY))
            {
                continue;
            }

            const bool diagonal = i >= 4;
            const std::uint64_t stepCost = MoveCost(map, nextX, nextY, diagonal);

            if (currentNode.cost > INF - stepCost)
            {
                continue;
            }

            const std::uint64_t newCost = currentNode.cost + stepCost;
            const std::size_t nextIndex = Index(nextX, nextY, width);

            if (newCost < distances[nextIndex])
            {
                distances[nextIndex] = newCost;
                openList.push({ newCost, nextX, nextY });
            }
        }
    }

    std::uint64_t bestCost = INF;
    std::shared_ptr<PointOfInterrest> bestVillage = nullptr;

    for (const auto& point : game->GetPointsOfInterest())
    {
        if (!point || point->GetEntityType() != EntityType::Village)
        {
            continue;
        }

        const CVect position = point->GetPosition();
        const int x = static_cast<int>(position.x);
        const int y = static_cast<int>(position.y);

        if (map->GetTileCost(x, y) == BLOCKED)
        {
            continue;
        }

        const std::uint64_t cost = distances[Index(x, y, width)];

        if (cost < bestCost)
        {
            bestCost = cost;
            bestVillage = point;
        }
    }

    return bestVillage;
}

void PlayerController::ComputePathUsingAStar(std::shared_ptr<Unit> unit, CVect to, bool allowDiags = false)
{
    if (!game || !unit)
    {
        return;
    }

    auto map = game->GetMap();

    if (!map)
    {
        return;
    }

    const int width = map->GetWidth();
    const int height = map->GetHeight();

    if (width <= 0 || height <= 0)
    {
        unit->SetPath({});
        return;
    }

    const CVect start = unit->GetTile();
    const int startX = static_cast<int>(start.x);
    const int startY = static_cast<int>(start.y);
    const int targetX = static_cast<int>(to.x);
    const int targetY = static_cast<int>(to.y);

    if (map->GetTileCost(startX, startY) == BLOCKED || map->GetTileCost(targetX, targetY) == BLOCKED)
    {
        unit->SetPath({});
        return;
    }

    if (startX == targetX && startY == targetY)
    {
        unit->SetPath({});
        return;
    }

    const std::size_t cellCount = static_cast<std::size_t>(width) * height;
    std::vector<std::uint64_t> distances(cellCount, INF);
    std::vector<int> previous(cellCount, -1);
    PriorityQueue openList;

    distances[Index(startX, startY, width)] = 0;
    openList.push({ Heuristic(startX, startY, targetX, targetY, allowDiags), startX, startY });

    while (!openList.empty())
    {
        const Node currentNode = openList.top();
        openList.pop();

        const std::size_t currentIndex = Index(currentNode.x, currentNode.y, width);
        const std::uint64_t currentCost = distances[currentIndex];

        if (currentCost == INF || currentNode.cost != currentCost + Heuristic(currentNode.x, currentNode.y, targetX, targetY, allowDiags))
        {
            continue;
        }

        if (currentNode.x == targetX && currentNode.y == targetY)
        {
            break;
        }

        for (int i = 0; i < (allowDiags ? 8 : 4); ++i)
        {
            const int nextX = currentNode.x + directionX[i];
            const int nextY = currentNode.y + directionY[i];

            if (!CanMove(map, currentNode.x, currentNode.y, nextX, nextY))
            {
                continue;
            }

            const bool diagonal = i >= 4;
            const std::uint64_t stepCost = MoveCost(map, nextX, nextY, diagonal);

            if (currentCost > INF - stepCost)
            {
                continue;
            }

            const std::uint64_t newCost = currentCost + stepCost;
            const std::size_t nextIndex = Index(nextX, nextY, width);

            if (newCost >= distances[nextIndex])
            {
                continue;
            }

            const std::uint64_t estimatedCost = Heuristic(nextX, nextY, targetX, targetY, allowDiags);

            if (newCost > INF - estimatedCost)
            {
                continue;
            }

            distances[nextIndex] = newCost;
            previous[nextIndex] = static_cast<int>(currentIndex);
            openList.push({ newCost + estimatedCost, nextX, nextY });
        }
    }

    const std::size_t targetIndex = Index(targetX, targetY, width);

    if (distances[targetIndex] == INF)
    {
        unit->SetPath({});
        return;
    }

    std::vector<CVect> path;
    const int startIndex = static_cast<int>(Index(startX, startY, width));
    int currentIndex = static_cast<int>(targetIndex);

    while (currentIndex != startIndex)
    {
        if (currentIndex < 0)
        {
            unit->SetPath({});
            return;
        }

        path.emplace_back(static_cast<float>(currentIndex % width), static_cast<float>(currentIndex / width), 0.0f);
        currentIndex = previous[static_cast<std::size_t>(currentIndex)];
    }

    std::reverse(path.begin(), path.end());
    unit->SetPath(path);
}

void FlowFieldMap::GenerateFlowField(std::shared_ptr<Game> game, CVect cvect, bool useCamps = false, bool allowDiags = false)
{
    if (!game)
    {
        return;
    }

    auto map = game->GetMap();

    if (!map)
    {
        return;
    }

    const int width = map->GetWidth();
    const int height = map->GetHeight();

    if (width <= 0 || height <= 0)
    {
        return;
    }

    ResetMap(width, height);

    const std::size_t cellCount = static_cast<std::size_t>(width) * height;
    std::vector<std::uint64_t> distances(cellCount, INF);
    std::vector<bool> isDestination(cellCount, false);
    PriorityQueue openList;

    if (useCamps)
    {
        for (const auto& point : game->GetPointsOfInterest())
        {
            if (!point || point->GetEntityType() != EntityType::GoblinCamp)
            {
                continue;
            }

            const CVect position = point->GetPosition();
            const int x = static_cast<int>(position.x);
            const int y = static_cast<int>(position.y);

            if (map->GetTileCost(x, y) == BLOCKED)
            {
                continue;
            }

            const std::size_t index = Index(x, y, width);
            distances[index] = 0;
            isDestination[index] = true;
            openList.push({ 0, x, y });
        }
    }
    else
    {
        const int x = static_cast<int>(cvect.x);
        const int y = static_cast<int>(cvect.y);

        if (map->GetTileCost(x, y) != BLOCKED)
        {
            const std::size_t index = Index(x, y, width);
            distances[index] = 0;
            isDestination[index] = true;
            openList.push({ 0, x, y });
        }
    }

    while (!openList.empty())
    {
        const Node currentNode = openList.top();
        openList.pop();

        if (currentNode.cost != distances[Index(currentNode.x, currentNode.y, width)])
        {
            continue;
        }

        for (int i = 0; i < (allowDiags ? 8 : 4); ++i)
        {
            const int nextX = currentNode.x + directionX[i];
            const int nextY = currentNode.y + directionY[i];

            if (!CanMove(map, currentNode.x, currentNode.y, nextX, nextY))
            {
                continue;
            }

            const bool diagonal = i >= 4;
            const std::uint64_t stepCost = MoveCost(map, currentNode.x, currentNode.y, diagonal);

            if (currentNode.cost > INF - stepCost)
            {
                continue;
            }

            const std::uint64_t newCost = currentNode.cost + stepCost;
            const std::size_t nextIndex = Index(nextX, nextY, width);

            if (newCost < distances[nextIndex])
            {
                distances[nextIndex] = newCost;
                openList.push({ newCost, nextX, nextY });
            }
        }
    }

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const std::size_t index = Index(x, y, width);

            if (distances[index] == INF)
            {
                continue;
            }

            const std::uint64_t roundedCost = distances[index] / STRAIGHT + (distances[index] % STRAIGHT != 0);
            distanceMap[y][x] = static_cast<unsigned int>(std::min(roundedCost, static_cast<std::uint64_t>(BLOCKED) - 1));

            if (isDestination[index])
            {
                continue;
            }

            std::uint64_t bestCost = INF;
            int bestDirectionX = 0;
            int bestDirectionY = 0;

            for (int i = 0; i < (allowDiags ? 8 : 4); ++i)
            {
                const int nextX = x + directionX[i];
                const int nextY = y + directionY[i];

                if (!CanMove(map, x, y, nextX, nextY))
                {
                    continue;
                }

                const std::uint64_t remainingCost = distances[Index(nextX, nextY, width)];

                if (remainingCost == INF)
                {
                    continue;
                }

                const bool diagonal = i >= 4;
                const std::uint64_t stepCost = MoveCost(map, nextX, nextY, diagonal);

                if (remainingCost > INF - stepCost)
                {
                    continue;
                }

                const std::uint64_t newCost = remainingCost + stepCost;

                if (newCost < bestCost)
                {
                    bestCost = newCost;
                    bestDirectionX = directionX[i];
                    bestDirectionY = directionY[i];
                }
            }

            flowField[y][x] = CVect(static_cast<float>(bestDirectionX), static_cast<float>(bestDirectionY), 0.0f);
        }
    }
}