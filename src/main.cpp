#include "mesh.h"
#include "matrix.h"

int main()
{
    Mesh mesh;
    mesh.LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\cube.obj");

    mesh.ExportObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\cubenew.obj");

    mesh.AddRotation(lin_alg::Matrix<4>::Axis3D::x, 45);

    mesh.AddTranslation(0.5, sqrt(2)/2, 0);

    mesh.AddRotation(lin_alg::Matrix<4>::Axis3D::z, -55);

    mesh.AddTranslation(1, -1, 0);

    mesh.ExecuteTransformation();

    mesh.ExportObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\cubenew.obj");
};