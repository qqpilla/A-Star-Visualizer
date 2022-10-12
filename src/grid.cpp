#include "grid.h"

Grid::Grid()
{    
    // set up cells
    cells = std::vector<std::vector<Cell>>(G_Resolution_Side, std::vector<Cell>(G_Resolution_Side));
    float cellSize = W_Width / G_Resolution_Side;
    for (int i = 0; i < G_Resolution_Side; i++)
    {
        for (int j = 0; j < G_Resolution_Side; j++)
        {
            cells[i][j] = {{cellSize / 2.0f + i * cellSize,
                            cellSize / 2.0f + j * cellSize}};
        }
    }

    // set up grid
    int count = G_Resolution_Side - 1;
    horizontal_grid = std::vector<float>(count);
    vertical_grid = std::vector<float>(count);
    for (int i = 0; i < count; i++)
    {
        horizontal_grid[i] = cellSize * (i + 1);
        vertical_grid[i] = cellSize * (i + 1);
    }
}

const std::vector<float>& Grid::HorizontalGrid() const
{
    return horizontal_grid;
}

const std::vector<float>& Grid::VerticalGrid() const
{
    return horizontal_grid;
}