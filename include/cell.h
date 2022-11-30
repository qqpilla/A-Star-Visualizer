#pragma once

struct Cell
{
    struct Point
    {
        float x;
        float y;
    } center;

    int grid_column;
    int grid_row;
    bool is_free = true;

    bool operator==(const Cell &cell) const
    {
        return grid_column == cell.grid_column && grid_row == cell.grid_row;
    }
    
    bool operator!=(const Cell &cell) const
    {
        return grid_column != cell.grid_column || grid_row != cell.grid_row;
    }

    bool operator<(const Cell &cell) const
    {
        return grid_row == cell.grid_row ? grid_column < cell.grid_column : 
                                           grid_row < cell.grid_row;
    }

    bool operator>(const Cell &cell) const
    {
        return grid_row == cell.grid_row ? grid_column > cell.grid_column : 
                                           grid_row > cell.grid_row;
    }
};