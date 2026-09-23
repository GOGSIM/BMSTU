#pragma once

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

private:
    std::vector<Node> nodes_;
    std::vector<FiniteElement> elements_;
    std::vector<FiniteElement> boundaryElements_;   
}; 