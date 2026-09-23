#include "AneuMeshLoader.h"
#include <fstream>
#include <stdexcept>
#include <vector>

Mesh AneuMeshLoader::loadMesh(const std::string& fileName) const
{
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open mesh file: " + fileName);
    }

    std::size_t nodeCount{};
    std::size_t dimension{};

    if (!(file >> nodeCount >> dimension))
    {
        throw std::runtime_error("Invalid .aneu header");
    }

    if (nodeCount == 0 || dimension != 3)
    {
        throw std::runtime_error("Unsupported or invalid mesh dimensions");
    }

    std::vector<Node> nodes;
    nodes.reserve(nodeCount);

    for (std::size_t i = 0; i < nodeCount; ++i)
    {
        Node node;

        if (!(file >> node.x >> node.y >> node.z))
        {
            throw std::runtime_error("Invalid node data in .aneu file");
        }

        node.id = i + 1;
        node.isVertex = true;

        nodes.push_back(node);
    }

    std::size_t elementCount{};
    std::size_t nodesPerElement{};

    if (!(file >> elementCount >> nodesPerElement))
    {
        throw std::runtime_error("Invalid finite element header");
    }

    if (elementCount == 0 || nodesPerElement != 4)
    {
        throw std::runtime_error("Unsupported or invalid finite element data");
    }

    std::vector<FiniteElement> elements;
    elements.reserve(elementCount);

    for (std::size_t i = 0; i < elementCount; ++i)
    {
        FiniteElement element;

        if (!(file >> element.geometryId))
        {
            throw std::runtime_error("Invalid finite element data");
        }

        element.id = i + 1;
        element.nodeIds.reserve(nodesPerElement);

        for (std::size_t j = 0; j < nodesPerElement; ++j)
        {
            std::size_t nodeId{};

            if (!(file >> nodeId))
            {
                throw std::runtime_error("Invalid node ID in finite element");
            }

            if (nodeId == 0 || nodeId > nodeCount)
            {
                throw std::runtime_error("Finite element references invalid node ID");
            }

            element.nodeIds.push_back(nodeId);
        }

        elements.push_back(element);
    }

    std::size_t boundaryElementCount{};
    std::size_t nodesPerBoundaryElement{};

    if (!(file >> boundaryElementCount >> nodesPerBoundaryElement))
    {
        throw std::runtime_error("Invalid boundary element header");
    }

    if (boundaryElementCount == 0 || nodesPerBoundaryElement != 3)
    {
        throw std::runtime_error("Unsupported or invalid boundary element data");
    }

    std::vector<FiniteElement> boundaryElements;
    boundaryElements.reserve(boundaryElementCount);

    for (std::size_t i = 0; i < boundaryElementCount; ++i)
    {
        FiniteElement element;

        if (!(file >> element.geometryId))
        {
            throw std::runtime_error("Invalid boundary element data");
        }

        element.id = i + 1;
        element.nodeIds.reserve(nodesPerBoundaryElement);

        for (std::size_t j = 0; j < nodesPerBoundaryElement; ++j)
        {
            std::size_t nodeId{};

            if (!(file >> nodeId))
            {
                throw std::runtime_error("Invalid node ID in boundary element");
            }

            if (nodeId == 0 || nodeId > nodeCount)
            {
                throw std::runtime_error("Boundary element references invalid node ID");
            }

            element.nodeIds.push_back(nodeId);
        }

        boundaryElements.push_back(element);
    }

    return Mesh(
        std::move(nodes),
        std::move(elements),
        std::move(boundaryElements)
    );
}