#pragma once
#include <cstddef>
#include <functional>
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

struct Edge
{
    std::size_t firstNodeId{};
    std::size_t secondNodeId{};

    Edge(std::size_t nodeId1, std::size_t nodeId2)
        : firstNodeId(nodeId1 < nodeId2 ? nodeId1 : nodeId2),
          secondNodeId(nodeId1 < nodeId2 ? nodeId2 : nodeId1)
    {
    }

    bool operator==(const Edge& other) const
    {
        return firstNodeId == other.firstNodeId
            && secondNodeId == other.secondNodeId;
    }
};

struct EdgeHash
{
    std::size_t operator()(const Edge& edge) const
    {
        const std::size_t firstHash =
            std::hash<std::size_t>{}(edge.firstNodeId);

        const std::size_t secondHash =
            std::hash<std::size_t>{}(edge.secondNodeId);

        return firstHash ^ (secondHash << 1);
    }
};