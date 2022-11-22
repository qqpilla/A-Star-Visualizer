#include <iostream>

#include "searcher.h"
#include <algorithm>
#include <cmath>

#include "glad/glad.h"

float *ExtractCoords(std::vector<Cell> cells, std::size_t &size)
{
    float *coords = new float[cells.size() * 2];
    for (std::size_t i = 0; i < cells.size(); i++)
    {
        coords[i * 2] = Normalized(cells[i].center.x);
        coords[i * 2 + 1] = Normalized(cells[i].center.y);
    }

    size = cells.size() * 2 * sizeof(float);
    return coords;
}

Searcher::Searcher(const Grid *searched_grid)
{
    grid = searched_grid;
}

void Searcher::InitializePath()
{
    std::size_t coords_s;
    float *coords = grid->NormalizedDefaultCellCoords(coords_s);

    unsigned int indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glGenVertexArrays(1, &path_vao);
    glBindVertexArray(path_vao);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, coords_s + sizeof(path_color), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, coords_s, coords);
    glBufferSubData(GL_ARRAY_BUFFER, coords_s, sizeof(path_color), path_color);

    delete[] coords;

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)coords_s);
    glVertexAttribDivisor(1, G_Resolution_Side * G_Resolution_Side);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Searcher::SetPathOffsetsVbo(float *data, std::size_t data_size)
{
    glBindVertexArray(path_vao);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0); 
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void Searcher::Reset()
{
    path.resize(0);
    path_size = 0;

    start = nullptr;
    destination = nullptr;
    
    came_from.clear();
    cost.clear();
    
    opened.clear();
    closed.resize(0);

    glBindVertexArray(path_vao);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
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

    // finding shortest path
    opened.put(*start, 0, 0);
    while(!opened.empty())
    {
        Cell current = opened.get();
        if (current == *destination)
            break;

        SearchStep(current);
    }

    // building path
    if (came_from.find(*destination) == came_from.end())
    {
        std::cout << "NO PATH FOUND" << std::endl;
        return;
    }

    Cell step = came_from[*destination];
    while (came_from.find(step) != came_from.end())
    {
        path.push_back(step);
        step = came_from[step];
    }

    // path is in reversed order right now (destination -> start)
    // so reversing it would be nice
    std::reverse(path.begin(), path.end());

    // passing path coords to new vbo
    path_size = path.size();

    std::size_t coords_s;
    float *coords = ExtractCoords(path, coords_s);
    SetPathOffsetsVbo(coords, coords_s);

    delete[] coords;
}

void Searcher::DrawPath() const
{
    glBindVertexArray(path_vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, path_size);
    glBindVertexArray(0);
}