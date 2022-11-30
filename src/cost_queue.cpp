#include "searcher.h"
#include <algorithm>

Cell Searcher::CostQueue::get()
{
    Cell el = std::get<Cell>(elements.back());
    elements.pop_back();
    return el;
}

const bool Searcher::CostQueue::empty() const
{
    return elements.empty();
}

void Searcher::CostQueue::clear()
{
    elements.clear();
}

void Searcher::CostQueue::put_unique(const Cell &item, int all_cost, int d_cost)
{
    auto it = std::find_if(elements.begin(), elements.end(), [&item](const CQElement &el) {return std::get<Cell>(el) == item;});
    
    if (it == elements.end())
    {
        elements.push_back({all_cost, d_cost, item});
    }
    else
    {
        std::get<0>(*it) = all_cost;
        std::get<1>(*it) = d_cost;
    }
}

void Searcher::CostQueue::sort()
{
    std::sort(elements.begin(), elements.end(), std::greater<CQElement>());
}