#include "mesh.h"
#include "matrix.h"
#include "vector.h"
#include "sphere.h"
#include "camera.h"
#include "scene.h"
#include "boundingsphere.h"
#include "plane.h"
#include "directionallight.h"
#include "pointlight.h"

int main()
{
    std::cout << "Rendering image..." << std::endl;

    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 4});

    Camera cam(cam_up, cam_forward, cam_focal, 2);

    cam.InitialiseScreenSize(32.0 / 9.0, 2);

    Sphere *sphere1 = new Sphere({2, 0.5, -3.5 + 1.3, 1}, 2, {0.1, 0.7, 0.1});
    Sphere *sphere2 = new Sphere({2.5, -0.15, -1.4 + 1.3, 1}, 0.5, {0.7, 0.55, 0.1});
    Sphere *sphere3 = new Sphere({-2.79, 1.45, -1.8, 1}, 0.5, {0, 0.8, 0.5});
    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\cube.obj");

    mesh->AddRotation(lin_alg::x, 50);
    mesh->AddRotation(lin_alg::y, 45);

    mesh->AddTranslation(-3, 1.2, -2);

    mesh->ExecuteTransformation();

    mesh->SetColour({0.4, 0.4, 0.4});

    mesh->faces[9].colour = {0, 0, 1};
    mesh->faces[8].colour = {0, 0, 1};

    mesh->faces[6].colour = {1, 0, 0};
    mesh->faces[4].colour = {1, 0, 0};

    mesh->faces[10].colour = {0, 1, 0};
    mesh->faces[11].colour = {0, 1, 0};

    mesh->specular_component = 1;

    mesh->RecalculateNormals();

    sphere1->specular_component = 1;
    sphere2->specular_component = 0.5;
    sphere3->specular_component = 1;

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    DirectionalLight *light = new DirectionalLight({1, 1, 0.5}, 0.7);
    PointLight *light2 = new PointLight({-4.5, 2.7, -2}, 0.5);

    BoundingSphere *bound = new BoundingSphere(mesh);

    Scene scene(cam, {0, 0, 0}, 0.2);
    scene.AddObject(sphere1);
    scene.AddObject(sphere2);
    scene.AddObject(sphere3);
    scene.AddObject(bound);
    scene.AddObject(plane);
    scene.AddLightSource(light);
    scene.AddLightSource(light2);
    const char *filename = (".\\out\\render.png");
    scene.Render(filename, 1920*8, 1080*8);
};