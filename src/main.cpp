#include "mesh.h"
#include "matrix.h"
#include "vector.h"
#include "sphere.h"
#include "camera.h"
#include "scene.h"
#include "boundingsphere.h"

// void encodeOneStep(const char *filename, std::vector<unsigned char> &image, unsigned width, unsigned height)
// {
//     //Encode the image
//     unsigned error = lodepng::encode(filename, image, width, height, LodePNGColorType::LCT_RGBA);

//     //if there's an error, display it
//     if (error)
//         std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
// }

int main()
{
    const char *filename = ("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\render.png");

    Sphere *sphere = new Sphere({-0.3, 1, -3.7, 1}, 1, {0.7, 0.2, 0.1});
    
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 1});

    Camera cam(cam_up, cam_forward, cam_focal, 1);

    cam.InitialiseScreenSize(2, 2);

    Sphere *sphere2 = new Sphere({1.5, 1, -4, 1}, 2, {0.4, 0.9, 0});

    Sphere *sphere3 = new Sphere({-3.5, 3, -4, 1}, 0.5, {0.4, 0, 0.6});

    Mesh* mesh = new Mesh();

    mesh->LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\cube.obj");

    mesh->AddTranslation(-1.5, -2, -4);

    mesh->ExecuteTransformation();

    mesh->SetColour({0.4, 0.4, 0.4});

    BoundingSphere* bound = new BoundingSphere(mesh);

    Scene scene(cam, {0, 0, 0});
    scene.AddObject(sphere);
    scene.AddObject(sphere2);
    scene.AddObject(sphere3);
    scene.AddObject(bound);



    scene.Render(filename, 10000, 10000);
};