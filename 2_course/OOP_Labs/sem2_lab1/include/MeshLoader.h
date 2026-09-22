#pragma once

#include <string>
#include "Mesh.h"

class MeshLoader
{
public:
    virtual ~MeshLoader() = default;
    virtual Mesh loadMesh(const std::string& fileName) const = 0;
};