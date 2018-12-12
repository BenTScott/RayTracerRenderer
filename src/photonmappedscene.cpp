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

    // TODO: Implement final gathering - this will only work on diffuse surfaces.
    bool use_shadow_rays;
    colour += photon_map->GetIrradianceEsitimate(intersect, cam.camera_focalpoint - pos, 2000, true, lighting_model, use_shadow_rays) * 20.0;

    for (const auto &light : light_sources)
    {
        std::vector<Ray> lightrays = light->GetLightRays(pos, 50);
        for (Ray &ray : lightrays)
        {
            if (!use_shadow_rays || !InShadow(ray))
            {
                colour += lighting_model->GetDiffuseLighting(*light, intersect).Scale(1.0 / lightrays.size());
                colour += lighting_model->GetSpecularLighting(*light, intersect).Scale(1.0 / lightrays.size());
            }
        }
    }

    if (intersect.material.GetReflectionConstant() > 0 && depth < max_reflection_depth)
    {
        auto reflection_intersect = GetRayIntersect(lighting_model->GetReflectionRay(intersect));
        if (reflection_intersect)
        {
            colour += CalculateColourAtIntersect(*reflection_intersect, photon_map, depth + 1).Scale(intersect.material.GetReflectionConstant());
        }
    }

    if (intersect.material.GetRefractionConstant().Magnitude() > 0 && depth < max_reflection_depth)
    {
        auto refraction_intersect = GetRayIntersect(lighting_model->GetRefractionRay(intersect));
        if (refraction_intersect)
        {
            colour += CalculateColourAtIntersect(*refraction_intersect, photon_map, depth + 1).PointwiseMultiply(intersect.material.GetRefractionConstant());
        }
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
                photons.push_back(new Photon(all_intersects[i]->GetCorrectedPosition(), photon_ray.ray.direction, {0, 0, 0}, Photon::Shadow));
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
            next_ray = PhotonPathRay(lighting_model->GetRefractionRay(*intersect), photon_ray.intensity.PointwiseMultiply(intersect->material.GetRefractionConstant().Scale(1.0 / intersect->material.GetRefractionConstant().Average())));
        }
        else
        {
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
        // else
        // {
        //     photons.push_back(new Photon(intersect->GetCorrectedPosition(), photon_ray.ray.direction, photon_ray.intensity, Photon::Direct));
        // }

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
            next_ray = PhotonPathRay(lighting_model->GetRefractionRay(*intersect), photon_ray.intensity.PointwiseMultiply(intersect->material.GetRefractionConstant().Scale(1.0 / intersect->material.GetRefractionConstant().Average())));
        }
        else
        {
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
    PhotonMap *global_map = GetGlobalPhotonMap(50000);
    PhotonMap *caustic_map = GetCausticPhotonMap(500000);

    ThreadSafeImage *image = new ThreadSafeImage(resolution_width, resolution_height);

    std::thread threads[max_thread];

    std::vector<PixelTask> tasks;
    tasks.reserve(resolution_width * resolution_height);

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            tasks.push_back(PixelTask(i, j, sample_rate));
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

    return image;
};

void PhotonMappedScene::AddSampling(const char *filename, unsigned resolution_width, unsigned resolution_height, unsigned max_samples)
{
    this->sample_rate = max_samples;

    cam.InitialiseResolution(resolution_width, resolution_height);

    ThreadSafeImage *image = new ThreadSafeImage(filename, resolution_width, resolution_height);

    std::vector<PixelTask> tasks = GetSampleRates(image);

    TaskQueue<PixelTask> queue(tasks);

    std::thread threads[max_thread];

    queue.InitailiseMonitor(*monitor);

    PhotonMap *global_map = GetGlobalPhotonMap(global_photons);
    PhotonMap *caustic_map = GetCausticPhotonMap(500000);

    for (unsigned i = 0; i < max_thread; ++i)
    {
        threads[i] = std::thread(&PhotonMappedScene::PixelThreadTask, this, std::ref(queue), image, *global_map, *caustic_map);
    }

    for (unsigned i = 0; i < max_thread; ++i)
    {
        threads[i].join();
    }

    image->Encode(filename);

    delete image;
};

void PhotonMappedScene::PixelThreadTask(TaskQueue<PixelTask> &queue, ThreadSafeImage *image, PhotonMap global_map, PhotonMap caustic_map)
{
    PixelTask task;

    while (queue.TryDequeue(task))
    {
        std::vector<Ray> rays = cam.GetJitterRaySamples(task.x, task.y, task.sample_rate);

        lin_alg::Vector<3> colour;

        for (Ray &ray : rays)
        {
            std::shared_ptr<RayIntersect> closest = GetRayIntersect(ray);
            bool use_shadow_rays;
            colour += caustic_map.GetIrradianceEsitimate(*closest, cam.camera_focalpoint - closest->GetCorrectedPosition(), 800, true, lighting_model, use_shadow_rays) * 8.0;
            colour += CalculateColourAtIntersect(*closest, &global_map);
        }

        image->SetPixel(task.x, task.y, colour.Scale(1.0 / rays.size()).Bound());
    }
};

std::vector<PhotonMappedScene::PixelTask> PhotonMappedScene::GetSampleRates(RGBImage *first_pass)
{
    RGBImage *copy = new RGBImage(*first_pass);

    ApplyKernelTransformations(copy);

    copy->Encode("kerneled_image.png");

    std::vector<PixelTask> tasks;
    tasks.reserve(first_pass->Width() * first_pass->Height());

    for (unsigned i = 0; i < first_pass->Width(); i++)
    {
        for (unsigned j = 0; j < first_pass->Height(); j++)
        {
            double max = copy->GetPixel(i, j).Max();
            unsigned pixel_sample_rate = std::round(max * sample_rate);
            if (pixel_sample_rate > 1)
            {
                PixelTask task(i, j, pixel_sample_rate);
                tasks.push_back(task);
            }
        }
    }

    delete copy;

    return tasks;
};

void PhotonMappedScene::ApplyKernelTransformations(RGBImage *first_pass)
{
    lin_alg::Matrix<3> kernel1;
    lin_alg::Matrix<3> kernel2;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (i == 1 && j == 1)
            {
                kernel1(i, j) = 8;
            }
            else
            {
                kernel1(i, j) = -1;
            }
            kernel2(i, j) = 1;
        }
    }

    // Apply edge detection kernel
    first_pass->ApplyKernel(kernel1);

    // Blur
    first_pass->ApplyKernel(kernel2, 1.0 / 5.0, 5);
};