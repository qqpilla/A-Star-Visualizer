#pragma once

#include <vector>
#include <cstddef>
#include <unordered_map>
#include "grid.h"
#include "cell.h"

class Searcher
{
private:
    struct cell_hash
    {
        std::size_t operator() (const Cell &cell) const
        {
            std::size_t h1 = std::hash<float>()(cell.center.x);
            std::size_t h2 = std::hash<float>()(cell.center.y);

            return h1 ^ h2;
        }
    };

    struct CostQueue
    {
    private:
        typedef std::tuple<int, int, Cell> CQElement;
        std::vector<CQElement> elements;

    public:
        Cell get();
        const bool empty() const;
        void clear();
        void put_unique(const Cell &item, int all_cost, int d_cost);
        void sort();
    };

    bool is_searching = false;

    std::vector<Cell> path;
    std::size_t path_size = 0;

    const Cell *start;
    const Cell *destination;
    const Grid *grid;

    std::unordered_map<Cell, std::pair<Cell, int>, cell_hash> came_from;
    std::unordered_map<Cell, int, cell_hash> cost;

    CostQueue opened;
    std::vector<Cell> closed;

    unsigned int path_vao;
    unsigned int opened_vao;
    unsigned int closed_vao;

    unsigned int *opened_vbo;
    unsigned int *closed_vbo;
    std::size_t opened_vbo_size = 0;
    std::size_t closed_vbo_size = 0;
    std::size_t opened_cells_count = 0;
    std::size_t closed_cells_count = 0;

    float path_color[3] = {0.66f, 0.2f, 0.89f};
    float opened_color[3] = {0.94f, 0.93f, 0.4f};
    float closed_color[3] = {0.95f, 0.62f, 0.21f};

    void InitializeCellsVao(unsigned int& VAO, float *cells_color, std::size_t color_size);
    void SetPathOffsetsVbo(float *data, std::size_t data_size);
    void AppendToOffsetsVbo(unsigned int &VAO, unsigned int **VBO, std::size_t &vbo_size, float *data, std::size_t data_size);

    int Distance(const Cell &a, const Cell &b) const;
    void BuildPath();

public:
    Searcher(const Grid *searched_grid);
    bool IsSearching() const;
    void InitializeAll();

    void Reset();
    void StartSearch();
    void SearchStep();

    void DrawPath() const;
    void DrawClosedCells() const;
    void DrawOpenedCells() const;
};