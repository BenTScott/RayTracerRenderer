#include "photonmappedscene.h"
#include "threadsafeimage.h"
#include <thread>

lin_alg::Vector<3> PhotonMappedScene::CalculateColourAtIntersect(const RayIntersect &intersect, PhotonMap *photon_map, unsigned depth) const
{
    lin_alg::Vector<3> colour;

    if (intersect.material.IsEmmitter())
    {
        return intersect.material.GetDiffuseConstant();
    }

    lin_alg::Vector<3> pos = intersect.GetCorrectedPosition();

    for (const auto &light : light_sources)
    {
        std::vector<Ray> lightrays = light->GetLightRays(pos, 50);

        for (Ray &ray : lightrays)
        {
            if (!InShadow(ray))
            {
                colour += lighting_model->GetDiffuseLighting(*light, intersect).Scale(1.0 / lightrays.size());
                colour += lighting_model->GetSpecularLighting(*light, intersect).Scale(1.0 / lightrays.size());
            }
        }
    }

    // TODO: Implement final gathering - this will only work on diffuse surfaces.
    colour += photon_map->GetIrradianceEsitimate(intersect, cam.camera_focalpoint - pos, 3000, true, lighting_model)* 30.0;

    // lin_alg::Vector<3> indirect_estimate;
    // double radius;
    // std::vector<Photon *> closest_photons = photon_map->LocatePhotons(pos, 4000, radius);

    // for (Photon *photon_ptr : closest_photons)
    // {
    //     if (photon_ptr->direction.DotProduct(intersect.normal) < 0.0)
    //     {
    //         double filter = std::max(0.0, 1 - (photon_ptr->position - pos).Magnitude() / radius);
    //         indirect_estimate += lighting_model->EstimatedPhotonRadiance(*photon_ptr, intersect, cam.camera_focalpoint - pos) * filter;
    //     }
    // }

    // colour += indirect_estimate * (70.0 / (M_PI * std::pow(radius, 2.0)));

    if (intersect.material.GetReflectionConstant() > 0 && depth < max_reflection_depth)
    {
        auto reflection_intersect = GetRayIntersect(lighting_model->GetReflectionRay(intersect));
        if (reflection_intersect)
        {
            colour += CalculateColourAtIntersect(*reflection_intersect, photon_map, depth + 1).Scale(intersect.material.GetReflectionConstant());
        }
        //GetColour(lighting_model->GetReflectionRay(intersect), depth + 1).Scale(intersect.material.GetReflectionConstant());
    }

    if (intersect.material.GetRefractionConstant() > 0 && depth < max_reflection_depth)
    {
        auto refraction_intersect = GetRayIntersect(lighting_model->GetRefractionRay(intersect));
        if (refraction_intersect)
        {
            colour += CalculateColourAtIntersect(*refraction_intersect, photon_map, depth + 1).Scale(intersect.material.GetRefractionConstant());
        }
        //colour += GetColour(lighting_model->GetRefractionRay(intersect), depth + 1).Scale(intersect.material.GetRefractionConstant());
    }

    return colour.Bound();
};

#pragma region Photon Mapping Methods

PhotonMap *PhotonMappedScene::GetGlobalPhotonMap(unsigned number_of_photons)
{
    std::vector<PhotonPathRay> emmitted_rays;
    emmitted_rays.reserve(number_of_photons);

    double intensity_sum = 0;
    for (Light *light : light_sources)
    {
        intensity_sum += (light->intensity).Magnitude();
    }

    for (Light *light : light_sources)
    {
        unsigned light_photon_count = std::round((light->intensity.Magnitude() / intensity_sum) * number_of_photons);
        std::vector<PhotonPathRay> rays = light->GeneratePhotonRays(light_photon_count);
        emmitted_rays.insert(end(emmitted_rays), begin(rays), end(rays));
    }
    std::cout << emmitted_rays.size() << " global photons emmitted\n";
    std::vector<Photon *> traced_photons;
    traced_photons.reserve(emmitted_rays.size() * 2);

    traced_photons = TraceLightRays(emmitted_rays);
    std::cout << traced_photons.size() << " global photons stored\n";
    PhotonMap *pm = new PhotonMap(traced_photons);
    pm->ScalePhotons(1.0 / number_of_photons);
    return pm;
};

PhotonMap *PhotonMappedScene::GetCausticPhotonMap(unsigned number_of_photons_per_object)
{
    std::vector<PhotonPathRay> emmitted_rays;
    emmitted_rays.reserve(number_of_photons_per_object);

    double intensity_sum = 0;
    for (Light *light : light_sources)
    {
        intensity_sum += (light->intensity).Magnitude();
    }

    for (SceneObject *obj_ptr : objects)
    {
        for (Light *light : light_sources)
        {
            if (obj_ptr->material.GetTransmittedProbablity() > 0)
            {
                unsigned light_photon_count = std::round((light->intensity.Magnitude() / intensity_sum) * number_of_photons_per_object);
                std::vector<PhotonPathRay> rays = light->GeneratePhotonRays(light_photon_count, obj_ptr);
                emmitted_rays.insert(end(emmitted_rays), begin(rays), end(rays));
            }
        }
    }

    std::cout << emmitted_rays.size() << " caustic photons emmitted\n";
    std::vector<Photon *> traced_photons;
    traced_photons.reserve(emmitted_rays.size());

    for (const PhotonPathRay &photon_ray : emmitted_rays)
    {
        std::shared_ptr<RayIntersect> closest = GetRayIntersect(photon_ray.ray);
        if (closest)
        {
            GetCausticPhotonOutcome(traced_photons, photon_ray, closest);
        }
    }

    std::cout << traced_photons.size() << " caustic photons stored\n";
    PhotonMap *pm = new PhotonMap(traced_photons);
    pm->ScalePhotons(1.0 / emmitted_rays.size());
    return pm;
};

std::vector<Photon *> PhotonMappedScene::TraceLightRays(std::vector<PhotonPathRay> &photon_rays)
{
    std::vector<Photon *> photons;

    // TODO: Parallise (probs not necessary??)
    for (const PhotonPathRay &photon_ray : photon_rays)
    {
        std::vector<std::shared_ptr<RayIntersect>> all_intersects;

        for (SceneObject *obj_ptr : objects)
        {
            std::shared_ptr<RayIntersect> intersect = obj_ptr->Intersect(photon_ray.ray);
            if (intersect)
            {
                all_intersects.push_back(intersect);
            }
        }

        if (all_intersects.size() != 0)
        {
            auto intersect_cmp = [](std::shared_ptr<RayIntersect> a, std::shared_ptr<RayIntersect> b) { return a->t < b->t; };
            std::sort(begin(all_intersects), end(all_intersects), intersect_cmp);

            //TODO: Add shadow photons
            for (unsigned i = 1; i < all_intersects.size(); ++i)
            {
                // if ((all_intersects[i]->GetCorrectedPosition() - sphere_bound_centre).Magnitude() < sphere_bound_radius)
                // {
                //     //photons.push_back(new Photon(all_intersects[i]->GetCorrectedPosition(), photon_ray.ray.direction, {0, 0, 0}, Photon::Shadow));
                // }
            }

            GetPhotonOutcome(photons, photon_ray, all_intersects[0], true);
        }
    }

    return photons;
};

void PhotonMappedScene::GetCausticPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, int depth)
{
    if (photon_ray.intensity.Magnitude() == 0 || depth > 100 || intersect->material.IsEmmitter())
    {
        return;
    }

    Material::PhotonOutcome outcome = intersect->material.GetPhotonOutcome();

    PhotonPathRay next_ray;

    if (outcome == Material::Absorbed || outcome == Material::Reflected)
    {
        photons.push_back(new Photon(intersect->GetCorrectedPosition(), photon_ray.ray.direction, photon_ray.intensity, Photon::Indirect));
        return;
    }
    else
    {
        Material::TransmissionOutcome outcome = intersect->material.GetTransmissionOutcome();
        if (outcome == Material::Refracted)
        {
            //TODO: Move initialisation to lighting model
            //next_ray = PhotonPathRay(lighting_model->GetRefractionRay(*intersect), photon_ray.intensity.Scale(1.0/(intersect->material.GetRefractionConstant())));
            next_ray = PhotonPathRay(lighting_model->GetRefractionRay(*intersect), photon_ray.intensity);
        }
        else
        {
            //next_ray = PhotonPathRay(lighting_model->GetReflectionRay(*intersect), photon_ray.intensity.Scale(1.0/(intersect->material.GetReflectionConstant())));
            next_ray = PhotonPathRay(lighting_model->GetReflectionRay(*intersect), photon_ray.intensity);
        }
    }

    std::shared_ptr<RayIntersect> closest = GetRayIntersect(next_ray.ray);

    if (closest)
    {
        GetCausticPhotonOutcome(photons, next_ray, closest, depth + 1);
    }
};

void PhotonMappedScene::GetPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, bool is_direct, int depth)
{
    if (photon_ray.intensity.Magnitude() == 0 || depth > 100 || intersect->material.IsEmmitter())
    {
        return;
    }

    Material::PhotonOutcome outcome = intersect->material.GetPhotonOutcome();

    PhotonPathRay next_ray;
    bool next_ray_direct = is_direct;

    if (outcome == Material::Absorbed || outcome == Material::Reflected)
    {
        if (!is_direct)
        {
            photons.push_back(new Photon(intersect->GetCorrectedPosition(), photon_ray.ray.direction, photon_ray.intensity, Photon::Indirect));
        }

        if (outcome == Material::Absorbed)
        {
            return;
        }

        next_ray_direct = false;
        next_ray = lighting_model->GetRandomPhotonReflection(intersect, photon_ray);
    }
    else
    {
        Material::TransmissionOutcome outcome = intersect->material.GetTransmissionOutcome();
        if (outcome == Material::Refracted)
        {
            //TODO: Move initialisation to lighting model
            //next_ray = PhotonPathRay(lighting_model->GetRefractionRay(*intersect), photon_ray.intensity.Scale(1.0/(intersect->material.GetRefractionConstant())));
            next_ray = PhotonPathRay(lighting_model->GetRefractionRay(*intersect), photon_ray.intensity);
        }
        else
        {
            //next_ray = PhotonPathRay(lighting_model->GetReflectionRay(*intersect), photon_ray.intensity.Scale(1.0/(intersect->material.GetReflectionConstant())));
            next_ray = PhotonPathRay(lighting_model->GetReflectionRay(*intersect), photon_ray.intensity);
        }
    }

    std::shared_ptr<RayIntersect> closest = GetRayIntersect(next_ray.ray);

    if (closest)
    {
        GetPhotonOutcome(photons, next_ray, closest, next_ray_direct, depth + 1);
    }
};

#pragma endregion

RGBImage *PhotonMappedScene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    PhotonMap *global_map = GetGlobalPhotonMap(global_photons);
    PhotonMap *caustic_map = GetCausticPhotonMap(500000);

    ThreadSafeImage *image = new ThreadSafeImage(resolution_width, resolution_height);

    std::thread threads[max_thread];

    std::vector<PixelTask> tasks;
    tasks.reserve(resolution_width * resolution_height);

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            tasks.push_back(PixelTask(i, j));
        }
    }

    TaskQueue<PixelTask> queue(tasks);

    queue.InitailiseMonitor(*monitor);

    for (unsigned i = 0; i < max_thread; ++i)
    {
        threads[i] = std::thread(&PhotonMappedScene::PixelThreadTask, this, std::ref(queue), image, *global_map, *caustic_map);
    }

    for (unsigned i = 0; i < max_thread; ++i)
    {
        threads[i].join();
    }

#pragma region Old Code
// for (unsigned i = 0; i < resolution_width; ++i)
// {
//     std::cout << i << "\n";
//     for (unsigned j = 0; j < resolution_height; ++j)
//     {
//         // if (monitor)
//         // {
//         //     monitor->Increment();
//         // }

//         std::shared_ptr<RayIntersect> closest = nullptr;
//         //* FIND CLOSEST INTERSECT FOR THE CAMERA RAY *//
//         {
//             Ray ray = cam.GetRay(i, j);

//             for (const auto &obj : objects)
//             {
//                 std::shared_ptr<RayIntersect> intersect = obj->Intersect(ray);
//                 if (intersect && (!closest || intersect->t < closest->t))
//                 {
//                     closest = intersect;
//                 };
//             }

//             if (!closest)
//             {
//                 image->SetPixel(i, j, background);
//                 continue;
//             }
//         }
//         auto pos = closest->GetCorrectedPosition();

//         //TODO: Case if it is a light hit i.e. area light...
//         // TODO: Move to photon map getting the irradiance estimate.
//         double caustic_radius;
//         std::vector<Photon *> caustic_photons = caustic_map->LocatePhotons(pos, 3000, caustic_radius);

//         // Remove photons not in the disc
//         // for (auto photon_iter = closest_photons.begin(); photon_iter < closest_photons.end(); photon_iter++)
//         // {
//         //     if ((*photon_iter)->normal.DotProduct(closest->normal) < 0.001 || (*photon_iter)->direction.DotProduct(closest->normal) > 0.001)
//         //     {
//         //         closest_photons.erase(photon_iter);
//         //     }
//         // }

//         lin_alg::Vector<3> caustic_estimate;

//         for (Photon *photon_ptr : caustic_photons)
//         {
//             if (photon_ptr->direction.DotProduct(closest->normal) < 0.0)
//             {
//                 double filter = std::max(0.0, 1 - (photon_ptr->position - pos).Magnitude() / caustic_radius);
//                 caustic_estimate += lighting_model->EstimatedPhotonRadiance(*photon_ptr, *closest, cam.camera_focalpoint - pos) * filter;
//                 //radius = std::max(radius, (photon_ptr->position - pos).Magnitude());
//             }
//         }

//         caustic_estimate = caustic_estimate * (5.0 / (M_PI * std::pow(caustic_radius, 2.0)));
//         lin_alg::Vector<3> radiance = CalculateColourAtIntersect(*closest);

//         image->SetPixel(i, j, (radiance + caustic_estimate).Bound());
//         //global->SetPixel(i, j, indirect_estimate.Bound());
//         //caustic->SetPixel(i, j, caustic_estimate.Bound());

//         //std::cout << closest_photons.size() << "\n";
//         //std::cout << i << ", " << j << " - " << estimate[0] << ", " << estimate[1] << ", " << estimate[2] << " - " << radius << " - " << closest_photons.size() << "\n";

//         // if (closest_photons.size() == 1)
//         // {
//         //     auto result = std::find(std::begin(photons_rendered), std::end(photons_rendered), closest_photons[0]);
//         //     if (result == std::end(photons_rendered))
//         //     {
//         //         image->SetPixel(i, j, closest_photons[0]->intensity.Bound());
//         //         photons_rendered.push_back(closest_photons[0]);
//         //     }
//         //     else
//         //     {
//         //         image->SetPixel(i, j, background);
//         //     }
//         // }
//         // else
//         // {
//         //     image->SetPixel(i, j, background);
//         // }

//         continue;
//     }
// }
#pragma endregion

    return image;
};

void PhotonMappedScene::PixelThreadTask(TaskQueue<PixelTask> &queue, ThreadSafeImage *image, PhotonMap global_map, PhotonMap caustic_map)
{
    PixelTask task;

    while (queue.TryDequeue(task))
    {
        std::vector<Ray> rays = cam.GetJitterRaySamples(task.x, task.y, sample_rate);

        lin_alg::Vector<3> colour;

        for (Ray &ray : rays)
        {
            std::shared_ptr<RayIntersect> closest = GetRayIntersect(ray);
            colour += caustic_map.GetIrradianceEsitimate(*closest, cam.camera_focalpoint - closest->GetCorrectedPosition(), 100, true, lighting_model)* 8.0;
            colour += CalculateColourAtIntersect(*closest, &global_map);
        }

        image->SetPixel(task.x, task.y, colour.Scale(1.0 / rays.size()).Bound());
    }
}

// lin_alg::Vector<3> PhotonMappedScene::CalculateColourAtIntersect(const RayIntersect &intersect, unsigned depth) const
// {
//     return lin_alg::Vector<3>();
// };