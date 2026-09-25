#pragma once
#include <ostream>
#include <vector>
#include "MeshTypes.h"

class Mesh
{
public:
    Mesh(
        std::vector<Node> nodes,
        std::vector<FiniteElement> elements,
        std::vector<FiniteElement> boundaryElements
    );

    const std::vector<Node>& getNodes() const;
    const std::vector<FiniteElement>& getElements() const;
    const std::vector<FiniteElement>& getBoundaryElements() const;

    void printNode(std::ostream& out, const Node& node) const;

    void printFiniteElement(
        std::ostream& out,
        const FiniteElement& element
    ) const;

    std::vector<std::size_t> findTetrahedraByThreeNodes(
        std::size_t nodeId1,
        std::size_t nodeId2,
        std::size_t nodeId3
    ) const;

    std::vector<std::size_t> findTetrahedraByEdge(
        std::size_t nodeId1,
        std::size_t nodeId2
    ) const;

    std::vector<std::size_t> findTetrahedraByRegionId(
        std::size_t regionId
    ) const;

    std::vector<std::size_t> findBoundaryElementsByBoundaryId(
        std::size_t boundaryId
    ) const;

    std::vector<std::size_t> findBoundaryNodesByBoundaryId(
        std::size_t boundaryId
    ) const;

private:
    std::vector<Node> nodes_;
    std::vector<FiniteElement> elements_;
    std::vector<FiniteElement> boundaryElements_;
};