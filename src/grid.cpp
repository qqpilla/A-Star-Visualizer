#include "grid.h"
#include <algorithm>

#include "glad/glad.h"

float Normalized(float value)
{
    return (value / W_Side) * 2.0f - 1.0f;
}

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
        }
    }
}

void Grid::InitializeGrid()
{
    // set of lines - all vertical or all horizontal lines
    // each vertical line has ends coords (offset, -1) & (offset, 1)
    // each horizontal line has ends coords (-1, offset) & (1, offset)
    int vertices_count = (G_Resolution_Side - 1) * 2;
    int lines_count = G_Resolution_Side - 1;
    float ends[vertices_count];
    float offsets[lines_count];

    // -1, 1, -1, 1 ... 
    for (int i = 0; i < vertices_count; i++)
        ends[i] = (i % 2) * 2.0f - 1.0f;

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
    unsigned int indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glGenVertexArrays(1, &start_vao);
    glGenBuffers(1, &start_vbo);

    glBindVertexArray(start_vao);
    glBindBuffer(GL_ARRAY_BUFFER, start_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(start_data) + sizeof(start_color), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(start_data), sizeof(start_color), start_color);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(start_data)));
    glVertexAttribDivisor(1, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &destination_vao);
    glGenBuffers(1, &destination_vbo);

    glBindVertexArray(destination_vao);
    glBindBuffer(GL_ARRAY_BUFFER, destination_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(destination_data) + sizeof(destination_color), NULL, GL_STATIC_DRAW);
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

void Grid::UpdateCellDataStorage(const Cell *cell, float *data_storage)
{
    float half_cell_size = cell_size / 2.0f;
    data_storage[0] = data_storage[2] = Normalized(cell->center.x - half_cell_size);
    data_storage[4] = data_storage[6] = Normalized(cell->center.x + half_cell_size);
    data_storage[1] = data_storage[7] = Normalized(cell->center.y - half_cell_size);
    data_storage[3] = data_storage[5] = Normalized(cell->center.y + half_cell_size);
}

void Grid::UpdateCellVbo(unsigned int &VBO, float *data, std::size_t data_size)
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, data);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Grid::RemoveStartCell()
{
    start = nullptr;
    for (int i = 0; i < sizeof(start_data) / sizeof(float); i++)
        start_data[i] = NULL;
    UpdateCellVbo(start_vbo, start_data, sizeof(start_data));
}

void Grid::RemoveDestinationCell()
{
    destination = nullptr;
    for (int i = 0; i < sizeof(destination_data) / sizeof(float); i++)
        destination_data[i] = NULL;
    UpdateCellVbo(destination_vbo, destination_data, sizeof(destination_data));
}

Cell* Grid::FindCellAround(double position_x, double position_y)
{
    float half_cell_size = cell_size / 2.0f;
    std::vector<Cell>::iterator it;
    for (int i = 0; i < G_Resolution_Side; i++)
    {
        it = std::find_if(cells[i].begin(), cells[i].end(),
                        [position_x, position_y, half_cell_size](const Cell &c) -> bool
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

void Grid::SetStartCell(Cell *cell)
{
    if (start == cell)
        return;

    if (destination == cell)
        RemoveDestinationCell();

    start = cell;
    UpdateCellDataStorage(start, start_data);
    UpdateCellVbo(start_vbo, start_data, sizeof(start_data));
}

void Grid::SetDestinationCell(Cell *cell)
{
    if (destination == cell)
        return;

    if (start == cell)
        RemoveStartCell();

    destination = cell;
    UpdateCellDataStorage(destination, destination_data);
    UpdateCellVbo(destination_vbo, destination_data, sizeof(destination_data));
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