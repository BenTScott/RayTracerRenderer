#include "mesh.h"
#include "matrix.h"
#include "vector.h"
#include "sphere.h"
#include "camera.h"
#include "scene.h"
#include "boundingsphere.h"

int main()
{
    std::cout << "Rendering first image..." << std::endl;

    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 1});

    Camera cam(cam_up, cam_forward, cam_focal, 1);

    cam.InitialiseScreenSize(2, 2);

    Scene scene1(cam, {0.2, 0.2, 0.2});

    Sphere *sphere1 = new Sphere({0.75, 0.5, -4, 1}, 2.75, {0.1, 1, 0});

    scene1.AddObject(sphere1);

    const char *filename1 = (".\\out\\render1.png");
    scene1.Render(filename1, 5000, 5000);

    std::cout << "Rendering second image..." << std::endl;

    Sphere *sphere2 = new Sphere({1.5, 1, -4, 1}, 2, {0.1, 0.7, 0.1});

    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\cube.obj");

    mesh->AddTranslation(-1.5, -1.5, -2);

    mesh->ExecuteTransformation();

    mesh->faces[2].colour = {0, 0, 1};
    mesh->faces[3].colour = {0, 0, 1};

    mesh->faces[4].colour = {0, 1, 0};
    mesh->faces[6].colour = {0, 1, 0};

    mesh->faces[10].colour = {1, 0, 0};
    mesh->faces[11].colour = {1, 0, 0};

    BoundingSphere *bound = new BoundingSphere(mesh);

    Scene scene2(cam, {0.2, 0.2, 0.2});
    scene2.AddObject(sphere2);
    scene2.AddObject(bound);

    const char *filename2 = (".\\out\\render2.png");
    scene2.Render(filename2, 5000, 5000);
};