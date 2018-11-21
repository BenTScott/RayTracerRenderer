#include "mesh.h"
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
#include "meshoctree.h"
#include "adaptivesampledscene.h"
#include "multithreadedscene.h"
#include "2dshapes.h"

lin_alg::Vector<3> GetColourVector(unsigned char R, unsigned char G, unsigned char B)
{
    return lin_alg::Vector<3>({(double)R / 255.0, (double)G / 255.0, (double)B / 255.0});
};

std::unique_ptr<Scene> Diamond_Scene_Two_Light_Sources(unsigned max_thread)
{
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 4});

    Camera cam(cam_up, cam_forward, cam_focal, 2);
    cam.InitialiseScreenSize(32.0 / 9.0, 2);

    Sphere *sphere1 = new Sphere({2, 0.5, -2.2, 1}, 2, {0.1, 0.7, 0.1});
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

    MeshOctree *meshbound = new MeshOctree(mesh, 10);

    BoundingSphere *spherebound2 = new BoundingSphere(sphere1, sphere2);

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 100, SampledScene::Jitter, max_thread));

    LightingModel *model = new AmbientOcclusionLightingModel(0.2, 10, new BasicLightingModel(0.1, 200), *scene);

    scene->AddObject(meshbound);
    scene->AddObject(spherebound2);
    scene->AddObject(plane);
    scene->AddLightSource(light);
    scene->AddLightSource(light2);
    scene->SetLightingModel(model);

    return scene;
};

std::unique_ptr<Scene> Cube_Scene_Two_Light_Sources(unsigned max_thread)
{
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 4});

    Camera cam(cam_up, cam_forward, cam_focal, 2);
    cam.InitialiseScreenSize(32.0 / 9.0, 2);

    Sphere *sphere1 = new Sphere({2, 0.5, -3.5 + 1.3, 1}, 2, {0.1, 0.7, 0.1});
    Sphere *sphere2 = new Sphere({2.5, -0.15, -0.01, 1}, 0.5, GetColourVector(211, 42, 118));
    Sphere *sphere3 = new Sphere({-2.775, 1.49, -1.775, 1}, 0.5, {0, 0.8, 0.5});

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

    mesh->faces[2].colour = {1, 1, 0};
    mesh->faces[3].colour = {1, 1, 0};

    mesh->faces[5].colour = {0, 1, 1};
    mesh->faces[7].colour = {0, 1, 1};

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

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 1000, SampledScene::Random,max_thread));

    LightingModel *model = new AmbientOcclusionLightingModel(0.2, 55, new BasicLightingModel(0.1, 200), *scene);

    scene->AddObject(spherebound1);
    scene->AddObject(spherebound2);
    scene->AddObject(plane);
    scene->AddLightSource(light);
    scene->AddLightSource(light2);
    scene->SetLightingModel(model);

    return scene;
};

std::unique_ptr<Scene> Reflections_Refractions(unsigned max_thread)
{
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 4});

    Camera cam(cam_up, cam_forward, cam_focal, 2);
    cam.InitialiseScreenSize(32.0 / 9.0, 2);

    Sphere *sphere1 = new Sphere({2.5, 0.5, -3.5 + 1.3, 1}, 2, {0.1, 0.7, 0.1});
    Sphere *sphere2 = new Sphere({1.5, -0.15, 0.5, 1}, 0.5, {0, 0, 0});
    Sphere *sphere3 = new Sphere({-2.775, 1.49, -1.775, 1}, 0.5, {0, 0.8, 0.5});
    Sphere *sphere4 = new Sphere({-4, 1.2, -6.5, 1}, 2, GetColourVector(209, 252, 55));

    Sphere *mirror_ball = new Sphere({-1.6, 1, -4.5, 1}, 0.95, {0, 0, 0});
    mirror_ball->reflection_constant = 1;
    mirror_ball->specular_component = 1;

    sphere2->refraction_constant = 0.95;
    sphere2->refractive_index = 1.3;

    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\cube.obj");

    mesh->AddRotation(lin_alg::x, 50);
    mesh->AddRotation(lin_alg::y, 45);

    mesh->AddTranslation(-3, 1.2, -2);

    mesh->ExecuteTransformation();

    mesh->SetColour({0, 0, 0});

    mesh->refractive_index = 1.5;
    mesh->refraction_constant = 1;

    mesh->specular_component = 1;

    mesh->RecalculateNormals();

    sphere1->specular_component = 1;
    sphere2->specular_component = 1;
    sphere3->specular_component = 1;
    sphere4->specular_component = 1;

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    DirectionalLight *light = new DirectionalLight({1, 1, 0.5}, 0.6);
    PointLight *light2 = new PointLight({-4.2, 1.8, -2}, 0.4);

    BoundingSphere *meshbound = new BoundingSphere(mesh);

    BoundingSphere *spherebound1 = new BoundingSphere(meshbound, sphere3);
    BoundingSphere *spherebound2 = new BoundingSphere(sphere1, sphere2);
    BoundingSphere *spherebound3 = new BoundingSphere(sphere4, mirror_ball);
    BoundingSphere *spherebound4 = new BoundingSphere(spherebound1, spherebound3);

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 10, SampledScene::Random, max_thread));

    LightingModel *model = new AmbientOcclusionLightingModel(0.2, 55, new BasicLightingModel(0.1, 200), *scene);

    scene->AddObject(spherebound4);
    scene->AddObject(spherebound2);
    scene->AddObject(plane);
    scene->AddLightSource(light);
    scene->AddLightSource(light2);
    scene->SetLightingModel(model);

    return scene;
};

std::unique_ptr<Scene> Bunny(unsigned max_thread)
{
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0.5, 4});

    Camera cam(cam_up, cam_forward, cam_focal, 2);
    cam.InitialiseScreenSize(32.0 / 9.0, 2);

    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\bunny.obj");
    mesh->AddTranslation(0, -0.16, -2);
    mesh->ExecuteTransformation();
    mesh->SetColour(GetColourVector(244, 170, 66));
    mesh->specular_component = 1;

    DirectionalLight *light = new DirectionalLight({1, 1, 0.5}, 0.6);

    MeshOctree *meshbound = new MeshOctree(mesh, 50);

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 200, SampledScene::Random, max_thread));

    LightingModel *model = new AmbientOcclusionLightingModel(0.2, 50, new BasicLightingModel(0.1, 200), *scene);

    scene->AddObject(meshbound);
    scene->AddObject(plane);
    scene->AddLightSource(light);
    scene->SetLightingModel(model);

    return scene;
};

int main(int argc, char *argv[])
{
    unsigned max_thread = 4;
    if (argc >= 2)
    {
        max_thread = std::strtoul(argv[1], nullptr, 10);
    }

    auto scene1 = Diamond_Scene_Two_Light_Sources(max_thread);
    auto scene2 = Cube_Scene_Two_Light_Sources(max_thread);
    auto scene3 = Reflections_Refractions(max_thread);
    auto scene4 = Bunny(max_thread);

    scene4->AddMonitoring();

    const char *filename = (".\\out\\render.png");
    scene4->Render(filename, 1920, 1080);
};