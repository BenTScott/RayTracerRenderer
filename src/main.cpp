#include "mesh.h"
#include "matrix.h"
#include "vector.h"
#include "sphere.h"
#include "camera.h"
#include "scene.h"
#include "boundingsphere.h"

int main()
{
    const char *filename = ("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\render.png");
    
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 1});

    Camera cam(cam_up, cam_forward, cam_focal, 1);

    cam.InitialiseScreenSize(2, 2);

    Sphere *sphere = new Sphere({1.5, 1, -4, 1}, 2, {0.1, 0.7, 0.1});

    Mesh* mesh = new Mesh();

    mesh->LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\cube.obj");

    mesh->AddTranslation(-1.5, -1.5, -2);

    mesh->ExecuteTransformation();

    mesh->faces[2].colour = {0, 0, 1};    
    mesh->faces[3].colour = {0, 0, 1};

    mesh->faces[4].colour = {0, 1, 0};
    mesh->faces[6].colour = {0, 1, 0};

    mesh->faces[10].colour = {1, 0, 0};
    mesh->faces[11].colour = {1, 0, 0};

    BoundingSphere* bound = new BoundingSphere(mesh);

    Scene scene(cam, {0.2, 0.2, 0.2});
    scene.AddObject(sphere);
    scene.AddObject(bound);
    
    scene.Render(filename, 2000, 2000);
};