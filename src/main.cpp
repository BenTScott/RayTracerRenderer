#include "mesh.h"
#include "matrix.h"
#include "vector.h"
#include "sphere.h"
#include "camera.h"
#include "scene.h"

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
    // // Mesh mesh;
    // // mesh.LoadObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\cube.obj");

    // // mesh.ExportObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\cubenew.obj");

    // // mesh.AddRotation(lin_alg::Matrix<4>::Axis3D::x, 45);

    // // mesh.AddTranslation(0.5, sqrt(2)/2, 0);

    // // mesh.AddRotation(lin_alg::Matrix<4>::Axis3D::z, -55);

    // // mesh.AddTranslation(1, -1, 0);

    // // mesh.ExecuteTransformation();

    // // mesh.ExportObjectModel("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\cubenew.obj");

    const char *filename = ("C:\\Users\\benty\\Documents\\Projects\\C++\\bin\\render.png");

    // //generate some image
    // unsigned width = 512, height = 512;
    // std::vector<unsigned char> image;
    // image.resize(width * height * 4);
    // for (unsigned y = 0; y < height; y++)
    //     for (unsigned x = 0; x < width; x++)
    //     {
    //         image[4 * width * y + 4 * x + 0] = 255;
    //         image[4 * width * y + 4 * x + 1] = 255;
    //         image[4 * width * y + 4 * x + 2] = 0;
    //         image[4 * width * y + 4 * x + 3] = 255 - ((double)(x + y)/(width+height))*255.0;
    //     }

    // encodeOneStep(filename, image, width, height);

    lin_alg::Vector<4> pos({-1, 0, -3, 1});
    lin_alg::Vector<3> col({1, 0, 0});
    Sphere *sphere = new Sphere(pos, 1, col);
    
    lin_alg::Vector<3> cam_up({0, 1, 0});
    lin_alg::Vector<3> cam_forward({0, 0, -1});
    lin_alg::Vector<3> cam_focal({0, 0, 1});

    Camera cam(cam_up, cam_forward, cam_focal, 1);

    cam.InitialiseScreenSize(2, 2);
    
    Scene scene(cam);
    scene.AddObject(sphere);
    scene.Render(filename, 512, 512);
};