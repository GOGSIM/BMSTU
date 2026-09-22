#pragma once

#include <cstddef>
#include <vector>

struct Node
{
    std::size_t id{};
    double x{};
    double y{};
    double z{};
    bool isVertex{};
};

struct FiniteElement
{
    std::size_t id{};
    std::size_t geometryId{};
    std::vector<std::size_t> nodeIds{};
};
