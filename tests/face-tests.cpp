#include "catch2/catch.hpp"
#include "face.h"

TEST_CASE("Can caluclate face normal")
{
    Vertex v1(1, 0, 0);
    Vertex v2(-1, 0, 0);
    Vertex v3(0, 0, 1);

    Face face(&v1, &v2, &v3);
    
    face.CalculateNormal();
    
    lin_alg::Vector<3> expected({0, 1, 0});

    REQUIRE(face.normal == expected);
}