#pragma once

#include <vector>
#include <cstddef>
#include "constants.h"

struct Cell
{
    struct Point
    {
        float x;
        float y;
    } center;

    int grid_row;
    int grid_column;    
    bool is_free = true;
};

class Grid
{
private:
    std::vector<std::vector<Cell>> cells;
    float cell_size = W_Side / G_Resolution_Side;

    unsigned int grid_vao;

    Cell *start = nullptr;
    Cell *destination = nullptr;

    unsigned int start_vao;
    unsigned int start_vbo;
    unsigned int destination_vao;
    unsigned int destination_vbo;

    float start_data[8]; // x & y for all 4 corners
    float destination_data[8];
    float start_color[3] = {1.0f, 0.0f, 0.0f};
    float destination_color[3] = {0.0f, 0.0f, 1.0f};

    unsigned int blocked_cells_vao;
    unsigned int blocked_cells_vbo;

    float blocked_cells_color[3] = {0.0f, 1.0f, 0.0f};

    void UpdateMainCellDataStorage(const Cell *cell, float *data_storage);
    void UpdateMainCellVbo(unsigned int &VBO, float *data, std::size_t data_size);
    void UpdateBlockedCellsVbo(float *data, std::size_t data_size, std::size_t offset);

    void RemoveStartCell();
    void RemoveDestinationCell();
    void RemoveAllBlockedCells();

public:
    Grid();
    void InitializeGrid();
    void InitializeMainCells();
    void InitializeBlockedCells();

    Cell* FindCellAround(double position_x, double position_y);
    
    void SetStartCell(Cell *cell);
    void SetDestinationCell(Cell *cell);
    void PlaceBlockedCell(Cell *cell);
    void RemoveBlockedCell(Cell *cell);
    void ClearAll();

    void DrawSetOfGridLines() const;
    void DrawStart() const;
    void DrawDestination() const;
    void DrawBlockedCells() const;
};