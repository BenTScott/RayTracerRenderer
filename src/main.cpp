#include "mesh.h"
#include "matrix.h"
#include "vector.h"
#include "sphere.h"
#include "camera.h"
#include "scene.h"
#include "sampledscene.h"
#include "boundingsphere.h"
#include "plane.h"
#include "directionallight.h"
#include "pointlight.h"
#include "ambientocclusionlightingmodel.h"
#include "basiclightingmodel.h"
#include "adaptivesampledscene.h"
#include "multithreadedscene.h"
#include "2dshapes.h"

#ifndef MAX_THREAD_COUNT
#define MAX_THREAD_COUNT 4
#endif

lin_alg::Vector<3> GetColourVector(unsigned char R, unsigned char G, unsigned char B)
{
    return lin_alg::Vector<3>({(double)R / 255.0, (double)G / 255.0, (double)B / 255.0});
};

std::unique_ptr<Scene> Diamond_Scene_Two_Light_Sources()
{
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 4});

    Camera cam(cam_up, cam_forward, cam_focal, 2);
    cam.InitialiseScreenSize(32.0 / 9.0, 2);

    Sphere *sphere1 = new Sphere({2, 0.5, -3.5 + 1.3, 1}, 2, {0.1, 0.7, 0.1});
    Sphere *sphere2 = new Sphere({2.5, -0.15, -0.01, 1}, 0.5, GetColourVector(211, 42, 118));
    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\diamond.obj");

    mesh->AddRotation(lin_alg::y, 45);

    mesh->AddTranslation(-3, 1.2, -2);

    mesh->ExecuteTransformation();

    mesh->SetColour(GetColourVector(244, 170, 66));

    mesh->specular_component = 1;

    mesh->RecalculateNormals();

    sphere1->specular_component = 1;
    sphere2->specular_component = 0.5;

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    DirectionalLight *light = new DirectionalLight({1, 1, 0.5}, 0.6);
    PointLight *light2 = new PointLight({-4.2, 1.8, -2}, 0.4);

    BoundingSphere *meshbound = new BoundingSphere(mesh);

    BoundingSphere *spherebound2 = new BoundingSphere(sphere1, sphere2);

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 100, SampledScene::Jitter, (unsigned)MAX_THREAD_COUNT));

    LightingModel *model = new AmbientOcclusionLightingModel(0.2, 45, new BasicLightingModel(0.1, 200), *scene);

    scene->AddObject(meshbound);
    scene->AddObject(spherebound2);
    scene->AddObject(plane);
    scene->AddLightSource(light);
    scene->AddLightSource(light2);
    scene->SetLightingModel(model);

    return scene;
};

std::unique_ptr<Scene> Cube_Scene_Two_Light_Sources()
{
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 4});

    Camera cam(cam_up, cam_forward, cam_focal, 2);
    cam.InitialiseScreenSize(32.0 / 9.0, 2);

    Sphere *sphere1 = new Sphere({2, 0.5, -3.5 + 1.3, 1}, 2, {0.1, 0.7, 0.1});
    Sphere *sphere2 = new Sphere({2.5, -0.15, -0.01, 1}, 0.5, GetColourVector(211, 42, 118));
    Sphere *sphere3 = new Sphere({-2.79, 1.45, -1.8, 1}, 0.5, {0, 0.8, 0.5});

    Sphere *mirror_ball = new Sphere({-3, 1.2, -4, 1}, 1, {0, 0, 0});
    mirror_ball->reflected_proportion = 0.95;

    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\cube.obj");

    mesh->AddRotation(lin_alg::x, 50);
    mesh->AddRotation(lin_alg::y, 45);

    mesh->AddTranslation(-3, 1.2, -2);

    mesh->ExecuteTransformation();

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

    DirectionalLight *light = new DirectionalLight({1, 1, 0.5}, 0.6);
    PointLight *light2 = new PointLight({-4.2, 1.8, -2}, 0.4);

    BoundingSphere *meshbound = new BoundingSphere(mesh);

    BoundingSphere *spherebound1 = new BoundingSphere(meshbound, sphere3);
    BoundingSphere *spherebound2 = new BoundingSphere(sphere1, sphere2);

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 10, SampledScene::Random, (unsigned)MAX_THREAD_COUNT));

    LightingModel *model = new AmbientOcclusionLightingModel(0.2, 45, new BasicLightingModel(0.1, 200), *scene);

    //scene->AddObject(spherebound1);
    scene->AddObject(spherebound2);
    scene->AddObject(mirror_ball);
    scene->AddObject(plane);
    scene->AddLightSource(light);
    scene->AddLightSource(light2);
    scene->SetLightingModel(model);

    return scene;
};

int main(int argc, char *argv[])
{
    auto scene1 = Diamond_Scene_Two_Light_Sources();
    auto scene2 = Cube_Scene_Two_Light_Sources(); 

    scene2->AddMonitoring();

    const char *filename = (".\\out\\render.png");
    scene2->Render(filename, 1920 / 2, 1080 / 2);
};