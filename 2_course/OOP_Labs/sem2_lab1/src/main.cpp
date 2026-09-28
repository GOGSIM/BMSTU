#include <exception>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>
#include "AneuMeshLoader.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: program <mesh_file.aneu>\n";
        return 1;
    }

    try
    {
        const std::string fileName = argv[1];

        AneuMeshLoader loader;
        Mesh mesh = loader.loadMesh(fileName);

        std::cout << "Mesh file: " << fileName << '\n';
        std::cout << "Nodes: " << mesh.getNodes().size() << '\n';
        std::cout << "Finite elements: "
                  << mesh.getElements().size() << '\n';
        std::cout << "Boundary elements: "
                  << mesh.getBoundaryElements().size() << '\n';

        std::cout << '\n';

        mesh.printNode(
            std::cout,
            mesh.getNodes().front()
        );
        std::cout << '\n';

        mesh.printFiniteElement(
            std::cout,
            mesh.getElements().front()
        );
        std::cout << '\n';

        mesh.printFiniteElement(
            std::cout,
            mesh.getBoundaryElements().front()
        );
        std::cout << '\n';

        const std::vector<std::size_t> tetrahedraByThreeNodes =
            mesh.findTetrahedraByThreeNodes(13, 14, 10);

        std::cout
            << "\nTetrahedra containing nodes 13, 14, 10: ";

        for (const std::size_t elementId : tetrahedraByThreeNodes)
        {
            std::cout << elementId << ' ';
        }

        std::cout << '\n';

        const std::vector<std::size_t> tetrahedraByEdge =
            mesh.findTetrahedraByEdge(13, 14);

        std::cout << "Tetrahedra containing edge 13-14: ";

        for (const std::size_t elementId : tetrahedraByEdge)
        {
            std::cout << elementId << ' ';
        }

        std::cout << '\n';

        const std::vector<std::size_t> tetrahedraByRegion =
            mesh.findTetrahedraByRegionId(1);

        std::cout << "Tetrahedra in region 1: "
                  << tetrahedraByRegion.size()
                  << '\n';

        const std::vector<std::size_t> boundaryElements =
            mesh.findBoundaryElementsByBoundaryId(31);

        std::cout << "Boundary elements on boundary 31: "
                  << boundaryElements.size()
                  << '\n';

        const std::vector<std::size_t> boundaryNodes =
            mesh.findBoundaryNodesByBoundaryId(31);

        std::cout << "Boundary nodes on boundary 31: "
                  << boundaryNodes.size()
                  << '\n';

        std::cout << "Node IDs: ";

        for (const std::size_t nodeId : boundaryNodes)
        {
            std::cout << nodeId << ' ';
        }

        std::cout << '\n';

        const std::vector<std::unordered_set<std::size_t>>
            adjacency = mesh.buildNodeAdjacency();

        std::cout << "Neighbors of node 13: ";

        for (const std::size_t neighborId : adjacency[13])
        {
            std::cout << neighborId << ' ';
        }

        std::cout << '\n';

        std::cout << "\nBefore midpoint insertion:\n";

        std::cout << "Nodes: "
                  << mesh.getNodes().size()
                  << '\n';

        std::cout << "First tetrahedron nodes: "
                  << mesh.getElements().front().nodeIds.size()
                  << '\n';

        std::cout << "First boundary element nodes: "
                  << mesh.getBoundaryElements()
                         .front()
                         .nodeIds
                         .size()
                  << '\n';

        mesh.insertMidpointNodes();

        std::cout << "\nAfter midpoint insertion:\n";

        std::cout << "Nodes: "
                  << mesh.getNodes().size()
                  << '\n';

        std::cout << "First tetrahedron nodes: "
                  << mesh.getElements().front().nodeIds.size()
                  << '\n';

        std::cout << "First boundary element nodes: "
                  << mesh.getBoundaryElements()
                         .front()
                         .nodeIds
                         .size()
                  << '\n';

        mesh.printFiniteElement(
            std::cout,
            mesh.getElements().front()
        );
        std::cout << '\n';

        mesh.printFiniteElement(
            std::cout,
            mesh.getBoundaryElements().front()
        );
        std::cout << '\n';
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Error: "
                  << exception.what()
                  << '\n';

        return 1;
    }

    return 0;
}