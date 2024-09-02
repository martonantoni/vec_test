#include <array>
#include <iostream>
#include <algorithm>
#include <vector>

#include "../common/allocation_counter.h"
#include "../common/progress_bar.h"
#include "../common/performance_measurer.h"

cLogPerformance_Guard performanceGuard("main");

template<class T> using TestedVec = std::vector<T, tAllocationCounter<T>>;

enum class Terrain
{
    Tree, Grass, Swamp
};

static constexpr std::array<std::pair<int, int>, 4> neighbours = { {{-1, 0}, {0, -1}, {1, 0}, {0, 1}} };

class Map
{
public:
    Terrain terrain(int row, int col) const;
};

class Solver
{
    const Map& mMap;
    struct cNode
    {
        int r, c;
        int cost;
        int hcost;
    };
public:
    Solver(const Map& map): mMap(map) {}
    int quickestPath(int startRow, int startCol, int targetRow, int targetCol);
};

int Solver::quickestPath(int start_row, int start_col, int target_row, int target_col)
{
    cProgressBar progressBar(abs(start_row - target_row) + abs(start_col - target_col));
    int bestDistance = abs(start_row - target_row) + abs(start_col - target_col);

    auto heuristic = [=](int r, int c) -> int
        {
            return abs(r - target_row) + abs(c - target_col);
        };
    TestedVec<cNode> open;
    TestedVec<std::pair<int, int>> closed;
    open.emplace_back(start_row, start_col, 0);
    while (!open.empty())
    {
        cNode current = open.front();
        int distance = abs(current.r - target_row) + abs(current.c - target_col);
        if (distance < bestDistance)
        {
            bestDistance = distance;
            progressBar.step();
        }
        std::ranges::pop_heap(open, std::greater<>{}, &cNode::hcost);
        open.pop_back();
        if (current.r == target_row && current.c == target_col)
        {
            progressBar.done();
            std::cout << "allocation count: " << tAllocationCounter<cNode>::allocationCount() << std::endl;
            return current.cost;
        }
        for (auto [dr, dc] : neighbours)
        {
            int r = current.r + dr;
            int c = current.c + dc;
            if (mMap.terrain(r, c) == Terrain::Tree)
            {
                continue;
            }
            auto closed_i = std::ranges::lower_bound(closed, std::pair{r, c});
            if (closed_i != closed.end() && *closed_i == std::pair{r, c})
            {
                continue;
            }

            int cost = current.cost + (mMap.terrain(current.r, current.c) == Terrain::Swamp ? 5 : 1);
            auto open_i = std::ranges::find_if(open, [=](const cNode& node) { return node.r == r && node.c == c; });
            if (open_i == open.end() || open_i->cost > cost)
            {
                open.emplace_back(r, c, cost, cost + heuristic(r, c));
                std::ranges::push_heap(open, std::greater<>{}, & cNode::hcost);
            }
        }
        auto closed_i = std::ranges::lower_bound(closed, std::pair{current.r, current.c});
        if (closed_i == closed.end() || *closed_i != std::pair{current.r, current.c})
        {
            closed.insert(closed_i, std::pair{current.r, current.c});
        }
    }
    return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////

Terrain Map::terrain(int row, int col) const
{
    auto firstPass = [](int row, int col) -> Terrain
        {
            int a = (row * 53 + col * 97) % 23;
            int b = (row * 67 + col * 41) % 29;
            int c = (row * 79 + col * 61) % 31;
            int d = (row * 89 + col * 71) % 37;
            int e = (row * 101 + col * 83) % 41;

            switch ((a ^ b ^ c ^ d ^ e) % 17)
            {
            case 1:
                return Terrain::Tree;
            case 2:
                return Terrain::Swamp;
            default:
                return Terrain::Grass;
            }
        };
    if (firstPass(row, col) == Terrain::Tree)
    {
        return Terrain::Tree;
    }
    for (auto&& [dr, dc] : neighbours)
    {
        if (firstPass(row + dr, col + dc) == Terrain::Tree)
        {
            return Terrain::Tree;
        }
    }
    for (auto&& [dr, dc] : neighbours)
    {
        if (firstPass(row + dr, col + dc) == Terrain::Swamp)
        {
            return Terrain::Swamp;
        }
    }
    return Terrain::Grass;
}

void printArea(const Map& map, int startRow, int startCol, int markedRow, int markedCol)
{
    for (int row = startRow; row < startRow + 120; ++row)
    {
        for (int col = startCol; col < startCol + 120; ++col)
        {
            if (row == markedRow && col == markedCol)
            {
                printf("X");
                continue;
            }
            switch (map.terrain(row, col))
            {
            case Terrain::Tree:
                printf("T");
                break;
            case Terrain::Grass:
                printf(".");
                break;
            case Terrain::Swamp:
                printf("+");
                break;
            }
        }
        printf("\n");
    }
}

int main()
{
    Map map;
    Solver solver(map);
  //  printArea(map, 1790, 1790, 1800, 1800);
    auto result = solver.quickestPath(1000, 1000, 1800, 1800);
    std::cout << "result: " << result;
}