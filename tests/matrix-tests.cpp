#include "catch2/catch.hpp"
#include "matrix.h"
#include "vector.h"

TEST_CASE("Can init matrix with array")
{
    double array[2][2] = {{1,2},{3, 4}};
    lin_alg::Matrix<2> matrix(array);

    REQUIRE(matrix(0,0) == 1);
    REQUIRE(matrix(1,0) == 3);
    REQUIRE(matrix(0,1) == 2);
    REQUIRE(matrix(1,1) == 4);
};

TEST_CASE("Default ctor initialises 0 matrix")
{
    double array[2][2] = {{0,0},{0, 0}};
    lin_alg::Matrix<2> matrix1;
    lin_alg::Matrix<2> matrix2(array);

    REQUIRE(matrix1 == matrix2);
};

TEST_CASE("Can initialise scalr matrix")
{
    double array[2][2] = {{2,0},{0, 2}};
    lin_alg::Matrix<2> matrix1(2);
    lin_alg::Matrix<2> matrix2(array);

    REQUIRE(matrix1 == matrix2);
};

TEST_CASE("Copy ctor copies values")
{
    double array[2][2] = {{2,0},{0, 2}};
    lin_alg::Matrix<2> matrix2(array);
    lin_alg::Matrix<2> matrix1(matrix2);

    REQUIRE(matrix1 == matrix2);
};

TEST_CASE("Can multiply two matricies")
{
    double array1[2][2] = {{1,2},{3, 4}};
    double array2[2][2] = {{4,3},{2, 1}};
    double expectedarray[2][2] = {{13.0,20.0},{5.0, 8.0}};
    lin_alg::Matrix<2> matrix1(array1);
    lin_alg::Matrix<2> matrix2(array2);
    lin_alg::Matrix<2> expected(expectedarray);

    REQUIRE((matrix1*matrix2) == expected);
};

TEST_CASE("Can left multiply matrix by a vector")
{
    double array1[2][2] = {{1,2},{3, 4}};
    lin_alg::Matrix<2> matrix(array1);
    lin_alg::Vector<2> vector({5,6});
    lin_alg::Vector<2> expected({23,34});

    REQUIRE((matrix*vector) == expected);
};