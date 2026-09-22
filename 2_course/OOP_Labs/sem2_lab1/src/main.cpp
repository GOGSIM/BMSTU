#include <iostream>
#include <string>

#include "AneuMeshLoader.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: program <mesh_file.aneu>\n";
        return 1;
    }

    const std::string fileName = argv[1];

    AneuMeshLoader loader;
    Mesh mesh = loader.loadMesh(fileName);

    std::cout << "Mesh file: " << fileName << '\n';

    return 0;
}


//.\build\Debug\sem2_lab1.exe .\MeshExample.aneu
