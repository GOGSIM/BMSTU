#include "Mesh.h"
#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <utility>

Mesh::Mesh(
    std::vector<Node> nodes,
    std::vector<FiniteElement> elements,
    std::vector<FiniteElement> boundaryElements
)
    : nodes_(std::move(nodes)),
      elements_(std::move(elements)),
      boundaryElements_(std::move(boundaryElements))
{
}

const std::vector<Node>& Mesh::getNodes() const
{
    return nodes_;
}

const std::vector<FiniteElement>& Mesh::getElements() const
{
    return elements_;
}

const std::vector<FiniteElement>& Mesh::getBoundaryElements() const
{
    return boundaryElements_;
}

void Mesh::printNode(std::ostream& out, const Node& node) const
{
    out << "Node { id: " << node.id
        << ", coordinates: ("
        << node.x << ", "
        << node.y << ", "
        << node.z << ")"
        << ", isVertex: "
        << (node.isVertex ? "true" : "false")
        << " }";
}

void Mesh::printFiniteElement(
    std::ostream& out,
    const FiniteElement& element
) const
{
    out << "FiniteElement { id: " << element.id
        << ", geometryId: " << element.geometryId
        << ", nodeIds: [";

    for (std::size_t i = 0; i < element.nodeIds.size(); ++i)
    {
        if (i > 0)
        {
            out << ", ";
        }

        out << element.nodeIds[i];
    }

    out << "] }";
}

std::vector<std::size_t> Mesh::findTetrahedraByThreeNodes(
    std::size_t nodeId1,
    std::size_t nodeId2,
    std::size_t nodeId3
) const
{
    std::vector<std::size_t> result;

    const auto containsAllThreeNodes =
        [nodeId1, nodeId2, nodeId3](const FiniteElement& element)
    {
        return
            std::find(
                element.nodeIds.cbegin(),
                element.nodeIds.cend(),
                nodeId1
            ) != element.nodeIds.cend()
            &&
            std::find(
                element.nodeIds.cbegin(),
                element.nodeIds.cend(),
                nodeId2
            ) != element.nodeIds.cend()
            &&
            std::find(
                element.nodeIds.cbegin(),
                element.nodeIds.cend(),
                nodeId3
            ) != element.nodeIds.cend();
    };

    auto current = elements_.cbegin();

    while (current != elements_.cend())
    {
        current = std::find_if(
            current,
            elements_.cend(),
            containsAllThreeNodes
        );

        if (current == elements_.cend())
        {
            break;
        }

        result.push_back(current->id);
        ++current;
    }

    return result;
}

std::vector<std::size_t> Mesh::findTetrahedraByEdge(
    std::size_t nodeId1,
    std::size_t nodeId2
) const
{
    std::vector<std::size_t> result;

    std::for_each(
        elements_.cbegin(),
        elements_.cend(),
        [nodeId1, nodeId2, &result](const FiniteElement& element)
        {
            const bool containsFirstNode =
                std::find(
                    element.nodeIds.cbegin(),
                    element.nodeIds.cend(),
                    nodeId1
                ) != element.nodeIds.cend();

            const bool containsSecondNode =
                std::find(
                    element.nodeIds.cbegin(),
                    element.nodeIds.cend(),
                    nodeId2
                ) != element.nodeIds.cend();

            if (containsFirstNode && containsSecondNode)
            {
                result.push_back(element.id);
            }
        }
    );

    return result;
}

std::vector<std::size_t> Mesh::findTetrahedraByRegionId(
    std::size_t regionId
) const
{
    std::vector<std::size_t> result;

    std::for_each(
        elements_.cbegin(),
        elements_.cend(),
        [regionId, &result](const FiniteElement& element)
        {
            if (element.geometryId == regionId)
            {
                result.push_back(element.id);
            }
        }
    );

    return result;
}

std::vector<std::size_t> Mesh::findBoundaryElementsByBoundaryId(
    std::size_t boundaryId
) const
{
    std::vector<std::size_t> result;

    std::for_each(
        boundaryElements_.cbegin(),
        boundaryElements_.cend(),
        [boundaryId, &result](const FiniteElement& element)
        {
            if (element.geometryId == boundaryId)
            {
                result.push_back(element.id);
            }
        }
    );

    return result;
}

std::vector<std::size_t> Mesh::findBoundaryNodesByBoundaryId(
    std::size_t boundaryId
) const
{
    std::vector<std::size_t> result;
    std::unordered_set<std::size_t> uniqueNodeIds;

    std::for_each(
        boundaryElements_.cbegin(),
        boundaryElements_.cend(),
        [boundaryId, &result, &uniqueNodeIds](const FiniteElement& element)
        {
            if (element.geometryId != boundaryId)
            {
                return;
            }

            std::for_each(
                element.nodeIds.cbegin(),
                element.nodeIds.cend(),
                [&result, &uniqueNodeIds](std::size_t nodeId)
                {
                    const auto insertionResult =
                        uniqueNodeIds.insert(nodeId);

                    if (insertionResult.second)
                    {
                        result.push_back(nodeId);
                    }
                }
            );
        }
    );

    return result;
}

std::vector<std::unordered_set<std::size_t>>
Mesh::buildNodeAdjacency() const
{
    std::vector<std::unordered_set<std::size_t>> adjacency(
        nodes_.size() + 1
    );

    std::for_each(
        elements_.cbegin(),
        elements_.cend(),
        [&adjacency](const FiniteElement& element)
        {
            for (std::size_t i = 0; i < element.nodeIds.size(); ++i)
            {
                for (
                    std::size_t j = i + 1;
                    j < element.nodeIds.size();
                    ++j
                )
                {
                    const std::size_t firstNodeId =
                        element.nodeIds[i];

                    const std::size_t secondNodeId =
                        element.nodeIds[j];

                    adjacency[firstNodeId].insert(secondNodeId);
                    adjacency[secondNodeId].insert(firstNodeId);
                }
            }
        }
    );

    return adjacency;
}

std::size_t Mesh::getOrCreateMidpointNode(
    const Edge& edge,
    std::unordered_map<Edge, std::size_t, EdgeHash>& edgeMidpoints
)
{
    const auto existingMidpoint = edgeMidpoints.find(edge);

    if (existingMidpoint != edgeMidpoints.end())
    {
        return existingMidpoint->second;
    }

    if (
        edge.firstNodeId == 0
        || edge.secondNodeId == 0
        || edge.firstNodeId > nodes_.size()
        || edge.secondNodeId > nodes_.size()
    )
    {
        throw std::out_of_range("Edge references invalid node ID");
    }

    const Node& firstNode = nodes_[edge.firstNodeId - 1];
    const Node& secondNode = nodes_[edge.secondNodeId - 1];

    Node midpoint;

    midpoint.id = nodes_.size() + 1;

    midpoint.x = (firstNode.x + secondNode.x) / 2.0;
    midpoint.y = (firstNode.y + secondNode.y) / 2.0;
    midpoint.z = (firstNode.z + secondNode.z) / 2.0;

    midpoint.isVertex = false;

    nodes_.push_back(midpoint);

    edgeMidpoints.emplace(edge, midpoint.id);

    return midpoint.id;
}

void Mesh::insertMidpointNodes()
{
    const bool invalidTetrahedron =
        std::any_of(
            elements_.cbegin(),
            elements_.cend(),
            [](const FiniteElement& element)
            {
                return element.nodeIds.size() != 4;
            }
        );

    const bool invalidBoundaryElement =
        std::any_of(
            boundaryElements_.cbegin(),
            boundaryElements_.cend(),
            [](const FiniteElement& element)
            {
                return element.nodeIds.size() != 3;
            }
        );

    if (invalidTetrahedron || invalidBoundaryElement)
    {
        throw std::logic_error(
            "Midpoint nodes can only be inserted into the original linear mesh"
        );
    }

    std::unordered_map<Edge, std::size_t, EdgeHash> edgeMidpoints;

    edgeMidpoints.reserve(
        elements_.size() * 6
        + boundaryElements_.size() * 3
    );

    const auto addMidpoints =
        [this, &edgeMidpoints](
            FiniteElement& element,
            std::size_t vertexCount
        )
    {
        std::vector<std::size_t> midpointIds;

        midpointIds.reserve(
            vertexCount * (vertexCount - 1) / 2
        );

        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            for (
                std::size_t j = i + 1;
                j < vertexCount;
                ++j
            )
            {
                const Edge edge(
                    element.nodeIds[i],
                    element.nodeIds[j]
                );

                const std::size_t midpointId =
                    getOrCreateMidpointNode(
                        edge,
                        edgeMidpoints
                    );

                midpointIds.push_back(midpointId);
            }
        }

        element.nodeIds.insert(
            element.nodeIds.end(),
            midpointIds.cbegin(),
            midpointIds.cend()
        );
    };

    std::for_each(
        elements_.begin(),
        elements_.end(),
        [&addMidpoints](FiniteElement& element)
        {
            addMidpoints(element, 4);
        }
    );

    std::for_each(
        boundaryElements_.begin(),
        boundaryElements_.end(),
        [&addMidpoints](FiniteElement& element)
        {
            addMidpoints(element, 3);
        }
    );
}