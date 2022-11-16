#include <iostream>

#include "searcher.h"
#include <algorithm>
#include <cmath>

Searcher::Searcher(const Grid *searched_grid)
{
    grid = searched_grid;
}

void Searcher::Reset()
{
    path.resize(0);

    start = nullptr;
    destination = nullptr;
    
    came_from.clear();
    cost.clear();
    
    opened.clear();
    closed.resize(0);
}

int Searcher::Distance(const Cell &a, const Cell &b) const
{
    return pow(a.grid_column - b.grid_column, 2) + pow(a.grid_row - b.grid_row, 2);
}

void Searcher::SearchStep(const Cell &current)
{
    closed.push_back(current);
    std::vector<Cell> neighbours = grid->FreeNeighbourCells(current);

    for (auto cur_nei : neighbours)
    {
        if (std::find(closed.begin(), closed.end(), cur_nei) != closed.end())
            continue;

        int d_cost = Distance(*destination, cur_nei);
        int new_cost = Distance(*start, cur_nei) + d_cost;

        if (cost.find(cur_nei) == cost.end() || 
            new_cost < cost[cur_nei])
        {
            cost[cur_nei] = new_cost;
            came_from[cur_nei] = current;
            opened.put(cur_nei, new_cost, d_cost);
        }
    }
}

void Searcher::FindPath()
{
    Reset();

    start = grid->Start();
    destination = grid->Destination();
    if (start == nullptr || destination == nullptr)
    {
        std::cout << "START AND/OR DESTINATION NOT SET" << std::endl;
        return;
    }

    // finding shortest pash
    opened.put(*start, 0, 0);
    while(!opened.empty())
    {
        Cell current = opened.get();
        if (current == *destination)
            break;

        SearchStep(current);
    }

    // building path
    Cell step = came_from[*destination];
    while (came_from.find(step) != came_from.end())
    {
        path.push_back(step);
        step = came_from[step];
    }

    // path is in reversed order right now (destination -> start)
    // so reversing it would be nice
    std::reverse(path.begin(), path.end());
}
