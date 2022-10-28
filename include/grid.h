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

    unsigned int blocked_vao;
    unsigned int blocked_vbo;

    float blocked_color[3] = {0.0f, 1.0f, 0.0f};

    void UpdateMainCellDataStorage(const Cell *cell, float *data_storage);
    void UpdateMainCellVbo(unsigned int &VBO, float *data, std::size_t data_size);
    void UpdateBlockedCellVbo(float *data, std::size_t data_size, int i_ind, int j_ind);

    void RemoveStartCell();
    void RemoveDestinationCell();

public:
    Grid();
    void InitializeGrid();
    void InitializeMainCells();
    void InitializeBlockedCells();

    Cell* FindCellAround(double position_x, double position_y, int &i_ind, int &j_ind);
    
    void SetStartCell(Cell *cell, int i_ind, int j_ind);
    void SetDestinationCell(Cell *cell, int i_ind, int j_ind);
    void PlaceBlockedCell(Cell *cell, int i_ind, int j_ind);
    void RemoveBlockedCell(Cell *cell, int i_ind, int j_ind);

    void DrawSetOfGridLines() const;
    void DrawStart() const;
    void DrawDestination() const;
    void DrawBlockedCells() const;
};