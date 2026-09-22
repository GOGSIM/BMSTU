#pragma once

#include <vector>

#include "MeshTypes.h"

class Mesh
{
public:
    const std::vector<Node>& getNodes() const;
    const std::vector<FiniteElement>& getElements() const;
    const std::vector<FiniteElement>& getBoundaryElements() const;

private:
    std::vector<Node> nodes_;
    std::vector<FiniteElement> elements_;
    std::vector<FiniteElement> boundaryElements_;   
}; 