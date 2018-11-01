#include "catch2/catch.hpp"
#include "vertex.h"

TEST_CASE("Can define a vertext with x, y and z values")
{
    Vertex v(1.2, 3.4, 5.6);

    REQUIRE(v.pos[0] == 1.2);
    REQUIRE(v.pos[1] == 3.4);
    REQUIRE(v.pos[2] == 5.6);
    REQUIRE(v.pos[3] == 1);
};

TEST_CASE("Can update position with vector")
{
    Vertex v(1, 2, 3);
    lin_alg::Vector<4> updatedpos({1.2, 3.4, 5.6, 1});
    v.Update_Position(updatedpos);

    REQUIRE(v.pos[0] == 1.2);
    REQUIRE(v.pos[1] == 3.4);
    REQUIRE(v.pos[2] == 5.6);
    REQUIRE(v.pos[3] == 1);
};

TEST_CASE("Can update position with transformation matrix")
{
    Vertex v(1, 2, 3);
    auto matrix = lin_alg::HomoTransMatrix4D(2, 3, 4);
    v.Transform(*matrix);

    REQUIRE(v.pos[0] == 3);
    REQUIRE(v.pos[1] == 5);
    REQUIRE(v.pos[2] == 7);
    REQUIRE(v.pos[3] == 1);
};