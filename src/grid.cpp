#include "grid.h"
#include <algorithm>
#include <string>

#include "glad/glad.h"

Grid::Grid()
{    
    // set up cells
    cells = std::vector<std::vector<Cell>>(G_Resolution_Side, std::vector<Cell>(G_Resolution_Side));
    float half_cell_size = cell_size / 2.0f;

    for (int i = 0; i < G_Resolution_Side; i++)
    {
        for (int j = 0; j < G_Resolution_Side; j++)
        {
            cells[i][j].center = {half_cell_size + i * cell_size,
                                  half_cell_size + j * cell_size};
            cells[i][j].grid_column = i;
            cells[i][j].grid_row = j;
        }
    }
}

void Grid::InitializeGrid()
{
    // set of lines - all vertical or all horizontal lines
    // each vertical line has ends coords (offset, -1) & (offset, 1)
    // each horizontal line has ends coords (-1, offset) & (1, offset)
    int lines_count = G_Resolution_Side - 1;
    float offsets[lines_count];
    float ends[2] = {-1.0f, 1.0f};

    for (int i = 0; i < lines_count; i++)
        offsets[i] = Normalized(cell_size * (i + 1));


    glGenVertexArrays(1, &grid_vao);
    glBindVertexArray(grid_vao);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ends) + sizeof(offsets), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ends), ends);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(ends), sizeof(offsets), offsets);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)(sizeof(ends)));
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Grid::InitializeMainCells()
{
    std::fill_n(start_data, 8, -1.0f);
    std::fill_n(destination_data, 8, -1.0f);

    unsigned int indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glGenVertexArrays(1, &start_vao);
    glBindVertexArray(start_vao);

    glGenBuffers(1, &start_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, start_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(start_data) + sizeof(start_color), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(start_data), start_data);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(start_data), sizeof(start_color), start_color);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(start_data)));
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &destination_vao);
    glBindVertexArray(destination_vao);

    glGenBuffers(1, &destination_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, destination_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(destination_data) + sizeof(destination_color), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(destination_data), destination_data);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(destination_data), sizeof(destination_color), destination_color);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(destination_data)));
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Grid::InitializeBlockedCells()
{
    std::size_t coords_s;
    float *coords = NormalizedDefaultCellCoords(coords_s);

    // by dafault no blocked cells should be drawn
    // so offset pushes the quads outside the window
    float offsets[2 * G_Resolution_Side * G_Resolution_Side];
    std::fill_n(offsets, 2 * G_Resolution_Side * G_Resolution_Side, Normalized(-cell_size / 2.0f));   


    unsigned int indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glGenVertexArrays(1, &blocked_cells_vao);
    glBindVertexArray(blocked_cells_vao);

    glGenBuffers(1, &blocked_cells_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, blocked_cells_vbo);
    glBufferData(GL_ARRAY_BUFFER, coords_s + sizeof(blocked_cells_color) + sizeof(offsets), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, coords_s, coords);
    glBufferSubData(GL_ARRAY_BUFFER, coords_s, sizeof(blocked_cells_color), blocked_cells_color);
    glBufferSubData(GL_ARRAY_BUFFER, coords_s + sizeof(blocked_cells_color), sizeof(offsets), offsets);

    delete[] coords;

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)coords_s);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(coords_s + sizeof(blocked_cells_color)));
    glVertexAttribDivisor(1, G_Resolution_Side * G_Resolution_Side);
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

const Cell* Grid::Start() const
{
    return start;
}

const Cell* Grid::Destination() const
{
    return destination;
}

void Grid::UpdateMainCellDataStorage(const Cell *cell, float *data_storage)
{
    float half_cell_size = cell_size / 2.0f;
    data_storage[0] = data_storage[2] = Normalized(cell->center.x - half_cell_size);
    data_storage[4] = data_storage[6] = Normalized(cell->center.x + half_cell_size);
    data_storage[1] = data_storage[7] = Normalized(cell->center.y - half_cell_size);
    data_storage[3] = data_storage[5] = Normalized(cell->center.y + half_cell_size);
}

void Grid::UpdateMainCellVbo(unsigned int &VBO, float *data, std::size_t data_size)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Grid::UpdateBlockedCellsVbo(float *data, std::size_t data_size, std::size_t offset)
{
    // + 11 comes from the fact that in the blocked_cells_vbo coords (8 floats)
    // and color (3 floats) of a single quad go first 
    offset += sizeof(float) * 11;

    glBindBuffer(GL_ARRAY_BUFFER, blocked_cells_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, offset, data_size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Grid::RemoveStartCell()
{
    start = nullptr;
    for (std::size_t i = 0; i < sizeof(start_data) / sizeof(float); i++)
        start_data[i] = -1.0f;
    UpdateMainCellVbo(start_vbo, start_data, sizeof(start_data));
}

void Grid::RemoveDestinationCell()
{
    destination = nullptr;
    for (std::size_t i = 0; i < sizeof(destination_data) / sizeof(float); i++)
        destination_data[i] = -1.0f;
    UpdateMainCellVbo(destination_vbo, destination_data, sizeof(destination_data));
}

void Grid::RemoveAllBlockedCells()
{
    for (std::size_t i = 0; i < cells.size(); i++)
        for (std::size_t j = 0; j < cells[i].size(); j++)
            if (!cells[i][j].is_free)
                cells[i][j].is_free = true;

    float offsets[2 * G_Resolution_Side * G_Resolution_Side];
    std::fill_n(offsets, 2 * G_Resolution_Side * G_Resolution_Side, Normalized(-cell_size / 2.0f));

    UpdateBlockedCellsVbo(offsets, sizeof(offsets), 0);
}

std::vector<Cell> Grid::ReachableFreeNeighbourCells(const Cell &cell) const
{
    std::vector<Cell> neighbours;
    for (int i = cell.grid_column - 1; i <= cell.grid_column + 1; i++)
    {
        for (int j = cell.grid_row - 1; j <= cell.grid_row + 1; j++)
        {
            if (i < 0 || i >= G_Resolution_Side || 
                j < 0 || j >= G_Resolution_Side || 
                (i == cell.grid_column && j == cell.grid_row) || 
                !cells[i][j].is_free ||
                // can't move diagonally if desired cell is blocked by 2 neighbours
                (i != cell.grid_column && j != cell.grid_row &&
                !cells[i][cell.grid_row].is_free && !cells[cell.grid_column][j].is_free)
            )
                continue;

            neighbours.push_back(cells[i][j]);
        }
    }

    return neighbours;
}

Cell* Grid::FindCellAround(double position_x, double position_y)
{
    float half_cell_size = cell_size / 2.0f;

    std::vector<Cell>::iterator it;
    for (int i = 0; i < G_Resolution_Side; i++)
    {
        it = std::find_if(cells[i].begin(), cells[i].end(),
            [position_x, position_y, half_cell_size](const Cell &c)
            {
                return position_x >= c.center.x - half_cell_size &&
                       position_x <= c.center.x + half_cell_size &&
                       position_y >= c.center.y - half_cell_size &&
                       position_y <= c.center.y + half_cell_size;
            });
        
        if (it != cells[i].end())
            break;
    }

    return &(*it);
}

float* Grid::NormalizedDefaultCellCoords(std::size_t &size) const
{
    // square with center point (0, 0)
    float half_cell = cell_size / 2.0f;
    float center = float(W_Side) / 2.0f;

    float *coords = new float[8]
    {
        Normalized(center - half_cell), Normalized(center - half_cell),
        Normalized(center - half_cell), Normalized(center + half_cell),
        Normalized(center + half_cell), Normalized(center + half_cell),
        Normalized(center + half_cell), Normalized(center - half_cell)
    };

    size = 8 * sizeof(float);
    return coords;
}

void Grid::SetStartCell(Cell *cell)
{
    if (start == cell)
        return;

    if (!cell->is_free)
        RemoveBlockedCell(cell);
    else if (destination == cell)
        RemoveDestinationCell();

    start = cell;
    UpdateMainCellDataStorage(start, start_data);
    UpdateMainCellVbo(start_vbo, start_data, sizeof(start_data));
}

void Grid::SetDestinationCell(Cell *cell)
{
    if (destination == cell)
        return;

    if (!cell->is_free)
        RemoveBlockedCell(cell);
    else if (start == cell)
        RemoveStartCell();

    destination = cell;
    UpdateMainCellDataStorage(destination, destination_data);
    UpdateMainCellVbo(destination_vbo, destination_data, sizeof(destination_data));
}

void Grid::PlaceBlockedCell(Cell* cell)
{
    if (!cell->is_free)
        return;

    if (start == cell)
        RemoveStartCell();
    else if (destination == cell)
        RemoveDestinationCell();

    cell->is_free = false;
    float position[2] = {Normalized(cell->center.x), Normalized(cell->center.y)};
    std::size_t offset = sizeof(float) * 2 * (cell->grid_row * G_Resolution_Side + cell->grid_column);

    UpdateBlockedCellsVbo(position, sizeof(position), offset);
}

void Grid::RemoveBlockedCell(Cell *cell)
{
    if (cell->is_free)
        return;

    cell->is_free = true;
    float position[2] = {Normalized(-cell_size / 2.0f), Normalized(-cell_size / 2.0f)};
    std::size_t offset = sizeof(float) * 2 * (cell->grid_row * G_Resolution_Side + cell->grid_column);

    UpdateBlockedCellsVbo(position, sizeof(position), offset);
}

void Grid::ClearAll()
{
    RemoveStartCell();
    RemoveDestinationCell();
    RemoveAllBlockedCells();
}

void Grid::DrawSetOfGridLines() const
{
    glBindVertexArray(grid_vao);
    glDrawArraysInstanced(GL_LINES, 0, 2, G_Resolution_Side - 1);
    glBindVertexArray(0);
}

void Grid::DrawStart() const
{
    glBindVertexArray(start_vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
    glBindVertexArray(0);
}

void Grid::DrawDestination() const
{
    glBindVertexArray(destination_vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
    glBindVertexArray(0);
}

void Grid::DrawBlockedCells() const
{
    glBindVertexArray(blocked_cells_vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, G_Resolution_Side * G_Resolution_Side);
    glBindVertexArray(0);
}