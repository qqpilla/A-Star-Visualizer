#pragma once

#include <vector>
#include "constants.h"

struct Cell
{
    struct Point
    {
        float x;
        float y;
    }  center;
};

class Grid
{
private:
    std::vector<std::vector<Cell>> cells;
    std::vector<float> horizontal_grid;
    std::vector<float> vertical_grid;
    //float vertical_grid[G_Resolution_Side - 1];


public:
    Grid();
    const std::vector<float>& HorizontalGrid() const;
    const std::vector<float>& VerticalGrid() const;
};