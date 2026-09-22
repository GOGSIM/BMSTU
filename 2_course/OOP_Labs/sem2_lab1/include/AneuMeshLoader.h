#pragma once

#include "MeshLoader.h"

class AneuMeshLoader : public MeshLoader
{
public:
    Mesh loadMesh(const std::string& fileName) const override;
    //намеренно переопределяю виртуальную функцию базового класса
};