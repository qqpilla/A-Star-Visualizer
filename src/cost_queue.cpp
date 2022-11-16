#include "searcher.h"

const bool Searcher::CostQueue::empty() const
{
    return elements.empty();
}

void Searcher::CostQueue::clear()
{
    elements = std::priority_queue<CQElement, std::vector<CQElement>, std::greater<CQElement>>();
}

void Searcher::CostQueue::put(Cell item, int all_cost, int d_cost)
{
    elements.emplace(all_cost, d_cost, item);
}

Cell Searcher::CostQueue::get()
{
    Cell el = std::get<Cell>(elements.top());
    elements.pop();
    return el;
}