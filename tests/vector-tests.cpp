#include "catch2/catch.hpp"
#include "vector.h"

TEST_CASE("Default vector init is zeroes", "[vector]")
{
    lin_alg::Vector<3> vector;

    REQUIRE(vector[0] == 0);
    REQUIRE(vector[1] == 0);
    REQUIRE(vector[2] == 0);
};

TEST_CASE("Vector can be init with double values using array init", "[vector]")
{
    double array[] = {0.5, 0.4, 0.3, 0.2, 0.1};
    lin_alg::Vector<5> vector(array);

    REQUIRE(vector[0] == 0.5);
    REQUIRE(vector[1] == 0.4);
    REQUIRE(vector[2] == 0.3);
    REQUIRE(vector[3] == 0.2);
    REQUIRE(vector[4] == 0.1);
};

TEST_CASE("Copy ctor conserves values", "[vector]")
{
    double array[] = {0.5, 0.4, 0.3, 0.2, 0.1};
    lin_alg::Vector<5> vector1(array);
    lin_alg::Vector<5> vector2(vector1);

    REQUIRE(vector1 == vector2);
};

TEST_CASE("Vector can be init with double values using list init", "[vector]")
{
    double array[] = {0.5, 0.4, 0.3, 0.2, 0.1};
    lin_alg::Vector<5> vector1(array);
    lin_alg::Vector<5> vector2({0.5, 0.4, 0.3, 0.2, 0.1});

    REQUIRE(vector1 == vector2);
};

TEST_CASE("Vectors can be added")
{
    lin_alg::Vector<3> vector1({0.3, 0.2, 0.1});
    lin_alg::Vector<3> vector2({0.1, 0.2, 0.3});
    lin_alg::Vector<3> expected({0.4, 0.4, 0.4});

    REQUIRE((vector1 + vector2) == expected);
};

TEST_CASE("Vectors can be minused")
{
    lin_alg::Vector<3> vector1({0.3, 0.2, 0.1});
    lin_alg::Vector<3> vector2({0.1, 0.2, 0.3});
    lin_alg::Vector<3> expected({0.3 - 0.1, 0, 0.1 - 0.3});

    REQUIRE((vector1 - vector2) == expected);
};

TEST_CASE("Can get the magnitude")
{
    lin_alg::Vector<2> vector({3, 4});

    REQUIRE(vector.Magnitude() == 5);
};

TEST_CASE("Can get the dot product")
{
    lin_alg::Vector<3> vector1({3, 4, 5});
    lin_alg::Vector<3> vector2({1, 2, 3});

    REQUIRE(vector1.DotProduct(vector2) == (3.0*1.0+4.0*2.0+5.0*3.0));
};

TEST_CASE("Can get the cross product for a 3D vector")
{
    lin_alg::Vector<3> vector1({3, 4, 5});
    lin_alg::Vector<3> vector2({1, 2, 3});
    lin_alg::Vector<3> expected({4.0*3.0-5.0*2.0, 5.0*1-3.0*3.0, 3.0*2.0-4.0*1.0});

    REQUIRE(lin_alg::CrossProduct(vector1, vector2) == expected);
};

TEST_CASE("Can get the cross product for a 4D homgeneous vector")
{
    lin_alg::Vector<4> vector1({3, 4, 5, 1});
    lin_alg::Vector<4> vector2({1, 2, 3, 1});
    lin_alg::Vector<4> expected({4.0*3.0-5.0*2.0, 5.0*1-3.0*3.0, 3.0*2.0-4.0*1.0, 1});

    REQUIRE(lin_alg::CrossProduct(vector1, vector2) == expected);
};

TEST_CASE("Can normalise a vector")
{
    lin_alg::Vector<2> vector({3, 4});
    lin_alg::Vector<2> expected({3.0/5.0, 4.0/5.0});

    vector.Normalise();

    REQUIRE(vector == expected);
};

TEST_CASE("Can get a 4D Vector as a Vector<3>")
{
    lin_alg::Vector<4> vector1({3, 4, 5, 1});
    lin_alg::Vector<3> expected({3, 4, 5});

    REQUIRE(vector1.GetAsVector3() == expected);
};

TEST_CASE("Can scale a vector")
{
    lin_alg::Vector<4> vector1({3, 4, 5, 1});
    lin_alg::Vector<4> expected({3.0*3.0, 4.0*3.0, 5.0*3.0, 1.0*3.0});

    REQUIRE(vector1.Scale(3) == expected);
};