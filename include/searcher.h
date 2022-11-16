#pragma once

#include <vector>
#include <cstddef>
#include <unordered_map>
#include <queue>
#include "grid.h"

class Searcher
{
private:
    struct cell_hash
    {
        std::size_t operator() (const Cell &cell) const
        {
            std::size_t h1 = std::hash<float>()(cell.center.x);
            std::size_t h2 = std::hash<float>()(cell.center.y);

            return h1 ^ h2;
        }
    };

    struct CostQueue
    {
    private:
        typedef std::tuple<int, int, Cell> CQElement;
        std::priority_queue<CQElement, std::vector<CQElement>, std::greater<CQElement>> elements;

    public:
        const bool empty() const;
        void clear();
        void put(Cell item, int all_cost, int d_cost);
        Cell get();
    };

    std::vector<Cell> path;
    const Cell *start;
    const Cell *destination;

    std::unordered_map<Cell, Cell, cell_hash> came_from;
    std::unordered_map<Cell, int, cell_hash> cost;

    CostQueue opened;
    std::vector<Cell> closed;

    const Grid *grid;

    void Reset();
    int Distance(const Cell &a, const Cell &b) const;
    void SearchStep(const Cell &current);

public:
    Searcher(const Grid *searched_grid);
    void FindPath();
};