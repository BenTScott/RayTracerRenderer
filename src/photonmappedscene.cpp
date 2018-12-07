#include "photonmappedscene.h"

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

std::vector<Photon *> PhotonMappedScene::TraceLightRays(std::vector<PhotonPathRay> &photon_rays)
{
    std::vector<Photon *> photons;

    //TODO: Parallise
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

            // Add shadow photons
            for (unsigned i = 1; i < all_intersects.size(); ++i)
            {
                if ((all_intersects[i]->GetCorrectedPosition() - sphere_bound_centre).Magnitude() < sphere_bound_radius)
                {
                    //photons.push_back(new Photon(all_intersects[i]->GetCorrectedPosition(), photon_ray.ray.direction, {0, 0, 0}, Photon::Shadow));
                }
            }

            GetPhotonOutcome(photons, photon_ray, all_intersects[0], true);

            // if (photon)
            // {
            //     // if ((photon->position - sphere_bound_centre).Magnitude() < sphere_bound_radius)
            //     // {
            //     photons.push_back(photon);
            //     // }
            //     // else
            //     // {
            //     //     delete photon;
            //     // }
            // }
        }
    }

    return photons;
};

void PhotonMappedScene::TracePhotonPath(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, int depth)
{
    std::shared_ptr<RayIntersect> closest = nullptr;

    for (SceneObject *obj_ptr : objects)
    {
        std::shared_ptr<RayIntersect> intersect = obj_ptr->Intersect(photon_ray.ray);
        if (intersect && (!closest || intersect->t < closest->t))
        {
            closest = intersect;
        }
    }

    if (!closest)
    {
        return;
    }

    GetPhotonOutcome(photons, photon_ray, closest, false, depth + 1);
};

void PhotonMappedScene::GetPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, bool is_direct, int depth)
{
    if (photon_ray.intensity.Magnitude() == 0 || depth > 100 || intersect->material.IsEmmitter())
    {
        return;
    }

    Material::PhotonOutcome outcome = intersect->material.GetPhotonOutcome();

    PhotonPathRay next_ray;

    if (outcome == Material::Absorbed)
    {

        Photon::Type type = Photon::Indirect;
        if (is_direct)
        {
            type = Photon::Direct;
            return; //nullptr;
        }
        photons.push_back(new Photon(intersect->GetCorrectedPosition(), photon_ray.ray.direction, photon_ray.intensity, Photon::Indirect));
        return;
        //return; // new Photon(intersect->GetCorrectedPosition(), photon_ray.ray.direction, photon_ray.intensity.Scale(1.0 / (1 - intersect->material.GetReflectionProbablity())), type);
    }
    else if (outcome == Material::Reflected)
    {
        photons.push_back(new Photon(intersect->GetCorrectedPosition(), photon_ray.ray.direction, photon_ray.intensity, Photon::Indirect));
        // std::uniform_real_distribution<> distribution(-1.0, 1.0);
        lin_alg::Vector<3> direction = Random::CosineHemisphereVector(intersect->normal);//RandomUnitVector();
        // // do
        // // {
        // //     direction = {distribution(Random::Generator()), distribution(Random::Generator()), distribution(Random::Generator())};
        // // } while(direction.Magnitude() > 1);

        // if (direction.DotProduct(intersect->normal) < 0)
        // {
        //     direction = direction.Scale(-1);
        // }
        // direction.Normalise();
        //next_ray = PhotonPathRay(Ray(intersect->GetCorrectedPosition(), direction), photon_ray.intensity.PointwiseMultiply(intersect->material.GetDiffuseConstant() * intersect->material.GetReflectionProbablity()));
        //next_ray = lighting_model->GetRandomPhotonRefle5ction(intersect, photon_ray);
        next_ray.ray = Ray(intersect->GetCorrectedPosition(), direction);
        next_ray.intensity = photon_ray.intensity.PointwiseMultiply(intersect->material.GetDiffuseConstant());
        next_ray.intensity = next_ray.intensity.Scale(1.0 / intersect->material.GetReflectionProbablity());
    }
    else
    {
        Material::TransmissionOutcome outcome = intersect->material.GetTransmissionOutcome();
        if (outcome == Material::Refracted)
        {
            //TODO: Move initialisation to lighting model
            next_ray = PhotonPathRay(lighting_model->GetRefractionRay(*intersect), photon_ray.intensity.Scale(intersect->material.GetRefractionConstant() / intersect->material.GetTransmittedProbablity()));
        }
        else
        {
            next_ray = PhotonPathRay(lighting_model->GetReflectionRay(*intersect), photon_ray.intensity.Scale(intersect->material.GetReflectionConstant() / intersect->material.GetTransmittedProbablity()));
        }
    }

    TracePhotonPath(photons, next_ray, depth);
};

RGBImage *PhotonMappedScene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    PhotonMap *global_map = GetGlobalPhotonMap(global_photons);

    auto image = new RGBImage(resolution_width, resolution_height);

    // monitor->Initialise(resolution_height * resolution_width);

    double max = 0.0;
    std::vector<Photon *> photons_rendered;
    double max1 = 0;
    //TODO: Parallise and make photon map thread safe
    for (unsigned i = 0; i < resolution_width; ++i)
    {
        std::cout << i << "\n";
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            // if (monitor)
            // {
            //     monitor->Increment();
            // }

            std::shared_ptr<RayIntersect> closest = nullptr;
            //* FIND CLOSEST INTERSECT FOR THE CAMERA RAY *//
            {
                Ray ray = cam.GetRay(i, j);

                for (const auto &obj : objects)
                {
                    std::shared_ptr<RayIntersect> intersect = obj->Intersect(ray);
                    if (intersect && (!closest || intersect->t < closest->t))
                    {
                        closest = intersect;
                    };
                }

                if (!closest)
                {
                    image->SetPixel(i, j, background);
                    continue;
                }
            }
            auto pos = closest->GetCorrectedPosition();

            //TODO: Case if it is a light hit i.e. area light...
            double radius;
            std::vector<Photon *> closest_photons = global_map->LocatePhotons(pos, 600, radius);

            // Remove photons not in the disc
            // for (auto photon_iter = closest_photons.begin(); photon_iter < closest_photons.end(); photon_iter++)
            // {
            //     if ((*photon_iter)->normal.DotProduct(closest->normal) < 0.001 || (*photon_iter)->direction.DotProduct(closest->normal) > 0.001)
            //     {
            //         closest_photons.erase(photon_iter);
            //     }
            // }

            lin_alg::Vector<3> estimate;
            //radius = 0;
            for (Photon *photon_ptr : closest_photons)
            {
                if (photon_ptr->direction.DotProduct(closest->normal) < 0.0)
                {
                    double filter = std::max(0.0, 1 - (photon_ptr->position - pos).Magnitude() / radius);
                    estimate += lighting_model->EstimatedPhotonRadiance(*photon_ptr, *closest, cam.camera_focalpoint - pos);// * filter;
                    //radius = std::max(radius, (photon_ptr->position - pos).Magnitude());
                }
            }

            max1 = std::max(estimate.Max(), max1);
            estimate = estimate * (8.0 / (M_PI * std::pow(radius, 2.0)));
            image->SetPixel(i, j, (CalculateColourAtIntersect(*closest) + estimate).Bound());

            //std::cout << closest_photons.size() << "\n";
            //std::cout << i << ", " << j << " - " << estimate[0] << ", " << estimate[1] << ", " << estimate[2] << " - " << radius << " - " << closest_photons.size() << "\n";

            // if (closest_photons.size() == 1)
            // {
            //     auto result = std::find(std::begin(photons_rendered), std::end(photons_rendered), closest_photons[0]);
            //     if (result == std::end(photons_rendered))
            //     {
            //         image->SetPixel(i, j, closest_photons[0]->intensity.Bound());
            //         photons_rendered.push_back(closest_photons[0]);
            //     }
            //     else
            //     {
            //         image->SetPixel(i, j, background);
            //     }
            // }
            // else
            // {
            //     image->SetPixel(i, j, background);
            // }

            continue;

            int direct_count = 0;
            int shadow_count = 0;
            int indirect_count = 0;

            lin_alg::Vector<3> indirect_illumination;
            //lin_alg::Vector<3> estimate;

            //TODO: Use count if for clarity??
            for (const auto &photon : closest_photons)
            {
                double filter = std::max(0.0, 1 - (photon->position - pos).Magnitude() / radius);
                if (photon->type == Photon::Shadow)
                {
                    shadow_count++;
                }
                if (photon->type == Photon::Direct)
                {
                    direct_count++;
                    //estimate += lighting_model->EstimatedPhotonRadiance(*photon, *closest, cam.camera_focalpoint - pos) * filter;
                }
                if (photon->type == Photon::Indirect)
                {
                    double filter = std::max(0.0, 1 - (photon->position - pos).Magnitude() / radius);
                    //indirect_illumination += lighting_model->EstimatedPhotonRadiance(photon, *closest, cam.camera_focalpoint - pos) * filter;
                    indirect_count++;
                    estimate += lighting_model->EstimatedPhotonRadiance(*photon, *closest, cam.camera_focalpoint - pos); // * filter;
                }
            }

            int total = shadow_count + direct_count;
            bool use_shadow_rays = true;

            if (shadow_count == 0 || (double)shadow_count / (double)total <= 0.05)
            {
                use_shadow_rays = false;
            }

            // if (indirect_count < 5)
            // {
            //     indirect_illumination = lin_alg::Vector<3>();
            //     //Calculate accuartely;
            // }
            // else
            // {
            //     indirect_illumination = indirect_illumination.Scale(1.0 / (M_PI * std::pow(radius, 2.0)));
            // }

            //indirect_illumination = indirect_illumination.Scale(3.0 / (M_PI * std::pow(radius, 2.0)));

            //image->SetPixel(i, j, estimate.Scale(3.0 / (M_PI * std::pow(radius, 2.0))).Bound());
            // if (indirect_count < 8)
            // {
            //     image->SetPixel(i, j, background);
            //     continue;
            // }

            estimate = estimate.Scale(1.0 / (M_PI * std::pow(radius, 2.0) * 300)); // * indirect_count));
            std::cout << i << ", " << j << " - " << estimate[0] << ", " << estimate[1] << ", " << estimate[2] << "\n";
            if (estimate.Max() > max)
            {
                max = estimate.Max();
            }
            //image->SetPixel(i, j, estimate.Bound());
            image->SetPixel(i, j, (CalculateColourAtIntersect(*closest) + estimate).Bound());
        }
    }
    std::cout << max1;
    return image;
};

// lin_alg::Vector<3> PhotonMappedScene::CalculateColourAtIntersect(const RayIntersect &intersect, unsigned depth) const
// {
//     return lin_alg::Vector<3>();
// };