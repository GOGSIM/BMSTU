#include "Mesh.h"
#include <utility>

Mesh::Mesh(
    std::vector<Node> nodes,
    std::vector<FiniteElement> elements,
    std::vector<FiniteElement> boundaryElements
)   //список инициализации конструктора
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