#include "Mesh.h"

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