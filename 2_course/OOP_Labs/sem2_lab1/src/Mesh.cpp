#include "Mesh.h"
#include <algorithm>
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