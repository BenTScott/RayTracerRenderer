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
    std::cout << "Rendering image..." << std::endl;

    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 4});

    Camera cam(cam_up, cam_forward, cam_focal, 2);

    cam.InitialiseScreenSize(32.0/9.0, 2);

    //Sphere *sphere = new Sphere({2, 1, -3.5, 1}, 2, {0.1, 0.7, 0.1});
    Sphere *sphere = new Sphere({2, 0.5, -3.5, 1}, 2, {0.1, 0.7, 0.1});

    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\cube.obj");

    mesh->AddTranslation(-3, 1.2, -2);

    mesh->ExecuteTransformation();

    mesh->faces[2].colour = {0, 0, 1};
    mesh->faces[3].colour = {0, 0, 1};

    mesh->faces[5].colour = {1, 0, 0};
    mesh->faces[7].colour = {1, 0, 0};

    mesh->faces[10].colour = {0, 1, 0};
    mesh->faces[11].colour = {0, 1, 0};

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    DirectionalLight *light1 = new DirectionalLight({1,1,0.5}, 0.7);
    DirectionalLight *light2 = new DirectionalLight({-1,1,1.5}, 0.3);

    BoundingSphere *bound = new BoundingSphere(mesh);

    Scene scene(cam, {0, 0, 0}, 0.2);
    scene.AddObject(sphere);
    scene.AddObject(bound);
    scene.AddObject(plane);
    scene.AddLightSource(light1);
    scene.AddLightSource(light2);
    const char *filename = (".\\out\\render.png");
    scene.Render(filename, 1920, 1080);
};