#include "mesh.h"
#include "matrix.h"
#include "vector.h"
#include "sphere.h"
#include "camera.h"
#include "scene.h"
#include "boundingsphere.h"
#include "plane.h"

int main()
{
    std::cout << "Rendering first image..." << std::endl;

    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 1});

    Camera cam(cam_up, cam_forward, cam_focal, 1);

    cam.InitialiseScreenSize(2, 2);

    Sphere *sphere = new Sphere({1.5, 1, -3.5, 1}, 2, {0.1, 0.7, 0.1});

    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\cube.obj");

    mesh->AddTranslation(-1.5, 1.2, -2);

    mesh->ExecuteTransformation();

    mesh->faces[2].colour = {0, 0, 1};
    mesh->faces[3].colour = {0, 0, 1};

    mesh->faces[5].colour = {1, 0, 0};
    mesh->faces[7].colour = {1, 0, 0};

    mesh->faces[10].colour = {0, 1, 0};
    mesh->faces[11].colour = {0, 1, 0};

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    DirectionalLight *light = new DirectionalLight({1, 2, 1}, 0.7);

    BoundingSphere *bound = new BoundingSphere(mesh);

    Scene scene(cam, {0, 0, 0}, 0.2);
    scene.AddObject(sphere);
    scene.AddObject(bound);
    scene.AddObject(plane);
    scene.AddLightSource(light);
    const char *filename = (".\\out\\render.png");
    scene.Render(filename, 512, 512);
};