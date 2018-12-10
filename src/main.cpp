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
#include "phonglightingmodel.h"
#include "meshoctree.h"
#include "adaptivesampledscene.h"
#include "multithreadedscene.h"
#include "2dshapes.h"
#include "photonmappedscene.h"
#include "arealight.h"

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

    mesh->material.SetSpecularConstant(1);

    mesh->RecalculateNormals();

    sphere1->material.SetSpecularConstant(1);
    sphere2->material.SetSpecularConstant(0.5);

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    DirectionalLight *light = new DirectionalLight({1, 1, 0.5}, 0.6);
    PointLight *light2 = new PointLight({-4.2, 1.8, -2}, 0.4);

    MeshOctree *meshbound = new MeshOctree(mesh, 10);

    BoundingSphere *spherebound2 = new BoundingSphere(sphere1, sphere2);

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 100, SampledScene::Jitter, max_thread));

    LightingModel *model = new AmbientOcclusionLightingModel(0.1, 200, 0.2, 10, *scene);

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

    mesh->faces[9].material.SetDiffuseConstant({0, 0, 1});
    mesh->faces[8].material.SetDiffuseConstant({0, 0, 1});

    mesh->faces[6].material.SetDiffuseConstant({1, 0, 0});
    mesh->faces[4].material.SetDiffuseConstant({1, 0, 0});

    mesh->faces[10].material.SetDiffuseConstant({0, 1, 0});
    mesh->faces[11].material.SetDiffuseConstant({0, 1, 0});

    mesh->faces[2].material.SetDiffuseConstant({1, 1, 0});
    mesh->faces[3].material.SetDiffuseConstant({1, 1, 0});

    mesh->faces[5].material.SetDiffuseConstant({0, 1, 1});
    mesh->faces[7].material.SetDiffuseConstant({0, 1, 1});

    mesh->material.SetSpecularConstant(1);

    mesh->RecalculateNormals();

    sphere1->material.SetSpecularConstant(1);
    sphere2->material.SetSpecularConstant(0.5);
    sphere3->material.SetSpecularConstant(1);

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    DirectionalLight *light = new DirectionalLight({1, 1, 0.5}, 0.6);
    PointLight *light2 = new PointLight({-4.2, 1.8, -2}, 0.4);

    BoundingSphere *meshbound = new BoundingSphere(mesh);

    BoundingSphere *spherebound1 = new BoundingSphere(meshbound, sphere3);
    BoundingSphere *spherebound2 = new BoundingSphere(sphere1, sphere2);

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 1000, SampledScene::Random, max_thread));

    LightingModel *model = new AmbientOcclusionLightingModel(0.1, 200, 0.2, 55, *scene);

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
    mirror_ball->material.AddReflection(1).SetSpecularConstant(1);

    sphere2->material.AddTransparency(0.95, 1.3);

    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\cube.obj");

    mesh->AddRotation(lin_alg::x, 50);
    mesh->AddRotation(lin_alg::y, 45);

    mesh->AddTranslation(-3, 1.2, -2);

    mesh->ExecuteTransformation();

    mesh->SetColour({0, 0, 0});

    mesh->material.AddTransparency(1, 1.5).SetSpecularConstant(1);

    mesh->RecalculateNormals();

    sphere1->material.SetSpecularConstant(1);
    sphere2->material.SetSpecularConstant(1);
    sphere3->material.SetSpecularConstant(1);
    sphere4->material.SetSpecularConstant(1);

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});

    DirectionalLight *light = new DirectionalLight({1, 1, 0.5}, 0.6);
    PointLight *light2 = new PointLight({-4.2, 1.8, -2}, 0.4);

    BoundingSphere *meshbound = new BoundingSphere(mesh);

    BoundingSphere *spherebound1 = new BoundingSphere(meshbound, sphere3);
    BoundingSphere *spherebound2 = new BoundingSphere(sphere1, sphere2);
    BoundingSphere *spherebound3 = new BoundingSphere(sphere4, mirror_ball);
    BoundingSphere *spherebound4 = new BoundingSphere(spherebound1, spherebound3);

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 10, SampledScene::Random, max_thread));

    LightingModel *model = new AmbientOcclusionLightingModel(0.1, 200, 0.2, 55, *scene);

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

    mesh->LoadObjectModel(".\\data\\bunny_with_normals.obj");
    mesh->AddTranslation(0, -0.16, -2);
    mesh->ExecuteTransformation();
    //mesh->SetColour(GetColourVector(244, 170, 66));
    mesh->material.SetSpecularConstant(1).AddTransparency(1, 1.5);

    DirectionalLight *light1 = new DirectionalLight({1, 1, 0.5}, 0.6);
    DirectionalLight *light2 = new DirectionalLight({-0.3, 1, -0.3}, 0.7);

    MeshOctree *meshbound = new MeshOctree(mesh, 50);

    Plane *plane = new Plane({0, 1, 0}, {0, -0.5, 0}, {0.4, 0.4, 0.4});
    Sphere *sphere1 = new Sphere({-4, 1.2, -6.5, 1}, 2, GetColourVector(209, 252, 55));
    Sphere *sphere2 = new Sphere({2, 0.5, -3.5, 1}, 1.5, {0.1, 0.7, 0.1});
    sphere1->material.SetSpecularConstant(1);
    sphere2->material.SetSpecularConstant(1);

    std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 200, SampledScene::Random, max_thread));

    LightingModel *model = new AmbientOcclusionLightingModel(0.1, 200, 0.2, 50, *scene);

    scene->AddObject(meshbound);
    scene->AddObject(sphere1);
    scene->AddObject(sphere2);

    scene->AddObject(plane);
    scene->AddLightSource(light1);
    scene->AddLightSource(light2);
    scene->SetLightingModel(model);

    return scene;
};

std::unique_ptr<Scene> CornellBox(unsigned max_thread)
{
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0.7, 1});

    Camera cam(cam_up, cam_forward, cam_focal, 0.8);
    cam.InitialiseScreenSize(2, 2);

    Plane *right_wall = new Plane({-1, 0, 0}, {2, 0, 0}, {0, 0.6, 0});
    right_wall->material.IntialiseRussianRoulette();

    Plane *left_wall = new Plane({1, 0, 0}, {-2, 0, 0}, {0.8, 0, 0});
    left_wall->material.IntialiseRussianRoulette();

    Plane *floor = new Plane({0, 1, 0}, {0, -0.8, 0}, {1, 1, 1});
    floor->material.IntialiseRussianRoulette();

    Plane *ceiling = new Plane({0, -1, 0}, {0, 2, 0}, {1, 1, 1});
    ceiling->material.IntialiseRussianRoulette();

    Plane *back_wall = new Plane({0, 0, 1}, {0, 0, -2.5}, {1, 1, 1});
    back_wall->material.IntialiseRussianRoulette();

    Plane *camera_wall = new Plane({0, 0, -1}, {0, 0, 5}, {0, 0, 0.7});
    camera_wall->material.IntialiseRussianRoulette();

    Mesh *mesh = new Mesh();

    mesh->LoadObjectModel(".\\data\\cube.obj");
    mesh->AddRotation(lin_alg::y, 35);
    mesh->AddTranslation(1, -0.3, -1.5);
    mesh->ExecuteTransformation();
    mesh->RecalculateNormals();
    mesh->SetColour({1, 1, 1});
    mesh->material.IntialiseRussianRoulette();

    lin_alg::Vector<3> centre = {0, 1.999, -0.8};
    Rectangle *light_rec = new Rectangle(centre + lin_alg::Vector<3>({-0.4, 0, 0.4}), centre + lin_alg::Vector<3>({-0.4, 0, -0.4}), centre + lin_alg::Vector<3>({0.4, 0, -0.4}), {1, 0, 0});
    AreaLight *area_light = new AreaLight(light_rec, {1, 1, 0.8});

    Sphere *sphere = new Sphere({-0.4, -0.15, -1, 1}, 0.6, {0, 0, 0});
    //sphere->material.SetSpecularConstant(0.05).IntialiseRussianRoulette();
    sphere->material.SetSpecularConstant(0.05).AddTransparency(0.95, 1.5).IntialiseRussianRoulette();

    LightingModel *model = new PhongLightingModel(0.1, 200);

    std::unique_ptr<Scene> scene(new PhotonMappedScene(cam, {0, 0, 0}, 100000, {0, 0.6, -1.5}, 6));
    //std::unique_ptr<Scene> scene(new MultithreadedScene(cam, {0, 0, 0}, 100, SampledScene::Jitter, max_thread));

    //LightingModel *model = new AmbientOcclusionLightingModel(0.1, 200, 0.3, 50, *scene);

    scene->SetLightingModel(model);
    scene->AddLightSource(area_light);
    scene->AddObject(right_wall);
    scene->AddObject(mesh);
    scene->AddObject(left_wall);
    scene->AddObject(back_wall);
    scene->AddObject(camera_wall);
    scene->AddObject(floor);
    scene->AddObject(ceiling);
    scene->AddObject(sphere);
    scene->AddObject(area_light);

    return scene;
}

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
    //auto scene4 = Bunny(max_thread);
    auto scene5 = CornellBox(max_thread);

    //scene3->AddMonitoring();

    const char *filename = (".\\out\\render2.png");
    scene5->Render(filename, 100, 100);
    //scene3->Render(filename, 1920, 1080);
};