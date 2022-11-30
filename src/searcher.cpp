#include <iostream>

#include "searcher.h"
#include <algorithm>
#include <cmath>

#include "glad/glad.h"

float *ExtractCoords(const std::vector<Cell> &cells, std::size_t &size)
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

float *ExtractCoords(const Cell &cell, std::size_t &size)
{
    float *coords = new float[2] 
    {
        Normalized(cell.center.x),
        Normalized(cell.center.y)
    };

    size = 2 * sizeof(float);
    return coords;
}

Searcher::Searcher(const Grid *searched_grid)
{
    grid = searched_grid;
}

bool Searcher::IsSearching() const
{
    return is_searching;
}

void Searcher::InitializeCellsVao(unsigned int& VAO, float *cells_color, std::size_t color_size)
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

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, coords_s + color_size, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, coords_s, coords);
    glBufferSubData(GL_ARRAY_BUFFER, coords_s, color_size, cells_color);

    delete[] coords;

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)coords_s);
    glVertexAttribDivisor(1, G_Resolution_Side * G_Resolution_Side);
    glVertexAttribDivisor(2, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Searcher::InitializeAll()
{
    InitializeCellsVao(path_vao, path_color, sizeof(path_color));
    InitializeCellsVao(opened_vao, opened_color, sizeof(opened_color));
    InitializeCellsVao(closed_vao, closed_color, sizeof(closed_color));
}

void Searcher::SetPathOffsetsVbo(float *data, std::size_t data_size)
{
    glBindVertexArray(path_vao);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0); 

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Searcher::AppendToOffsetsVbo(unsigned int &VAO, unsigned int **VBO, std::size_t &vbo_size, float *data, std::size_t data_size)
{
    unsigned int *new_vbo = new unsigned int;
    glGenBuffers(1, new_vbo);

    glBindBuffer(GL_COPY_WRITE_BUFFER, *new_vbo);
    glBufferData(GL_COPY_WRITE_BUFFER, vbo_size + data_size, NULL, GL_STATIC_DRAW);    
    glBufferSubData(GL_COPY_WRITE_BUFFER, vbo_size, data_size, data);
    if (vbo_size > 0)
    {
        glBindBuffer(GL_COPY_READ_BUFFER, **VBO);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, vbo_size);    
        glBindBuffer(GL_COPY_READ_BUFFER, 0);

        glDeleteBuffers(1, *VBO);
        delete *VBO;
    }
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

    *VBO = new_vbo;
    vbo_size += data_size;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, **VBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int Searcher::Distance(const Cell &a, const Cell &b) const
{
    return abs(a.grid_column - b.grid_column) + abs(a.grid_row - b.grid_row);
}

void Searcher::Reset()
{
    is_searching = false;

    path.resize(0);
    path_size = 0;

    start = nullptr;
    destination = nullptr;
    
    came_from.clear();
    cost.clear();
    
    opened.clear();
    closed.resize(0);

    if (opened_vbo_size > 0)
    {
        glDeleteBuffers(1, opened_vbo);
        delete opened_vbo;
        opened_vbo_size = 0;
        opened_cells_count = 0;
    }
    if (closed_vbo_size > 0)
    {
        glDeleteBuffers(1, closed_vbo);
        delete closed_vbo;
        closed_vbo_size = 0;
        closed_cells_count = 0;
    }
}

void Searcher::StartSearch()
{
    Reset();

    start = grid->Start();
    destination = grid->Destination();
    if (start == nullptr || destination == nullptr)
    {
        std::cout << "START AND/OR DESTINATION NOT SET" << std::endl;
        return;
    }

    opened.put_unique(*start, 0, 0);
    came_from[*start] = {*start, 0};
    is_searching = true;
}

void Searcher::SearchStep()
{
    if (!is_searching)
        return;

    bool destination_met = false;
    if (!opened.empty())
    {
        Cell current = opened.get();

        if (current == *destination)
        {
            destination_met = true;
        }
        else
        {
            closed.push_back(current);
           
            std::vector<Cell> all_neighbours = grid->ReachableFreeNeighbourCells(current);
            std::vector<Cell> added_neighbours;
            bool sort_needed = false;

            for (auto cur_nei : all_neighbours)
            {
                if (std::find(closed.begin(), closed.end(), cur_nei) != closed.end())
                    continue;
                
                int g_cost = Distance(current, cur_nei) + came_from[current].second;
                int h_cost = Distance(*destination, cur_nei);
                int f_cost = g_cost + h_cost;

                bool is_opened = cost.find(cur_nei) != cost.end();
                if (!is_opened || f_cost < cost[cur_nei])
                {
                    cost[cur_nei] = f_cost;
                    came_from[cur_nei] = {current, g_cost};
                    opened.put_unique(cur_nei, f_cost, h_cost);

                    if (!is_opened)
                        added_neighbours.push_back(cur_nei);

                    sort_needed = true;
                }
            }
            if (sort_needed)
                opened.sort();

            if (added_neighbours.size() > 0)
            {
                std::size_t opened_data_s;
                float *opened_data = ExtractCoords(added_neighbours, opened_data_s);
                AppendToOffsetsVbo(opened_vao, &opened_vbo, opened_vbo_size, opened_data, opened_data_s);
                opened_cells_count += added_neighbours.size();

                delete[] opened_data;
            }

            std::size_t closed_data_s;
            float *closed_data = ExtractCoords(current, closed_data_s);
            AppendToOffsetsVbo(closed_vao, &closed_vbo, closed_vbo_size, closed_data, closed_data_s);
            closed_cells_count++;
            delete[] closed_data;
        }
    }

    if (destination_met)
    {
        BuildPath();
        is_searching = false;
    }
    else if (opened.empty())
    {
        std::cout << "NO PATH FOUND" << std::endl;
        is_searching = false;
    }
}

void Searcher::BuildPath()
{
    Cell step = came_from[*destination].first;
    while (came_from[step].second != 0)
    {
        path.push_back(step);
        step = came_from[step].first;
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

void Searcher::DrawClosedCells() const
{
    glBindVertexArray(closed_vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, closed_cells_count);
    glBindVertexArray(0);
}

void Searcher::DrawOpenedCells() const
{
    glBindVertexArray(opened_vao);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, opened_cells_count);
    glBindVertexArray(0);
}