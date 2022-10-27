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
    }  center;
    
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

    void UpdateCellDataStorage(const Cell *cell, float *data_storage);
    void UpdateCellVbo(unsigned int &VBO, float *data, std::size_t data_size);
    void RemoveStartCell();
    void RemoveDestinationCell();

public:
    Grid();
    void InitializeGrid();
    void InitializeMainCells();

    Cell* FindCellAround(double position_x, double position_y);
    void SetStartCell(Cell *cell);
    void SetDestinationCell(Cell *cell);

    void DrawSetOfGridLines() const;
    void DrawStart() const;
    void DrawDestination() const;
};