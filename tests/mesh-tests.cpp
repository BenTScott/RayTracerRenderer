#include "catch2/catch.hpp"
#include "mesh.h"

TEST_CASE("Can load obj file")
{
    Mesh mesh;
    mesh.LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\tests\\test_data\\testmesh.obj");
    
    lin_alg::Vector<4> expected_v1({1, 0, 0, 1});
    lin_alg::Vector<4> expected_v2({-1, 0, 0, 1});
    lin_alg::Vector<4> expected_v3({0, 0, 1, 1});

    REQUIRE(mesh.vertices.size() == 3);
    REQUIRE(mesh.vertices[0].pos == expected_v1);
    REQUIRE(mesh.vertices[1].pos == expected_v2);
    REQUIRE(mesh.vertices[2].pos == expected_v3);

    REQUIRE(mesh.faces.size() == 1);

    mesh.faces[0].CalculateNormal();
    lin_alg::Vector<3> expected({0, 1, 0});
    REQUIRE(mesh.faces[0].normal == expected);
}

TEST_CASE("Can export obj file")
{
    // Values should be preserved in the export
    Mesh mesh1;
    mesh1.LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\tests\\test_data\\testmesh.obj");
    mesh1.ExportObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\tests\\test_data\\exporttestmesh.obj", true);
    
    Mesh mesh;
    mesh.LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\tests\\test_data\\exporttestmesh.obj");

    lin_alg::Vector<4> expected_v1({1, 0, 0, 1});
    lin_alg::Vector<4> expected_v2({-1, 0, 0, 1});
    lin_alg::Vector<4> expected_v3({0, 0, 1, 1});

    REQUIRE(mesh.vertices.size() == 3);
    REQUIRE(mesh.vertices[0].pos == expected_v1);
    REQUIRE(mesh.vertices[1].pos == expected_v2);
    REQUIRE(mesh.vertices[2].pos == expected_v3);

    REQUIRE(mesh.faces.size() == 1);

    mesh.faces[0].CalculateNormal();
    lin_alg::Vector<3> expected({0, 1, 0});
    REQUIRE(mesh.faces[0].normal == expected);
}

TEST_CASE("Can translate mesh")
{
    Mesh mesh;
    mesh.LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\tests\\test_data\\testmesh.obj");
    
    lin_alg::Vector<4> expected_v1({2, 2, 3, 1});
    lin_alg::Vector<4> expected_v2({0, 2, 3, 1});
    lin_alg::Vector<4> expected_v3({1, 2, 4, 1});

    mesh.Translate(1, 2, 3);

    REQUIRE(mesh.vertices[0].pos == expected_v1);
    REQUIRE(mesh.vertices[1].pos == expected_v2);
    REQUIRE(mesh.vertices[2].pos == expected_v3);
}

TEST_CASE("Can rotate mesh")
{
    Mesh mesh;
    mesh.LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\tests\\test_data\\testmesh.obj");
    
    lin_alg::Vector<4> expected_v1({1, 0, 0, 1});
    lin_alg::Vector<4> expected_v2({-1, 0, 0, 1});

    mesh.Rotate(lin_alg::x, -90);

    REQUIRE(mesh.vertices[0].pos == expected_v1);
    REQUIRE(mesh.vertices[1].pos == expected_v2);
    REQUIRE(mesh.vertices[2].pos[1] == 1);
}

TEST_CASE("Can set colour")
{
    Mesh mesh;
    mesh.LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\tests\\test_data\\testmesh.obj");

    lin_alg::Vector<3> colour({1,1,1});

    mesh.SetColour(colour);
    REQUIRE(mesh.faces[0].colour == colour);
}