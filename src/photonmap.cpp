#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include "photonmap.h"

#define M_PI 3.14159265358979323846

PhotonMap::Node::Node(std::vector<Photon *> &photons)
{
    if (photons.size() == 0)
    {
        return;
    }

    if (photons.size() > 1)
    {
        lin_alg::Vector<3> min({INFINITY, INFINITY, INFINITY});
        lin_alg::Vector<3> max({-INFINITY, -INFINITY, -INFINITY});
        for (const Photon *photon : photons)
        {
            for (int i = 0; i < 3; i++)
            {
                max[i] = std::max(max[i], photon->position[i]);
                min[i] = std::min(min[i], photon->position[i]);
            }
        }

        double dim_diff = -INFINITY;

        for (int i = 0; i < 3; i++)
        {
            if (max[i] - min[i] > dim_diff)
            {
                dim_diff = max[i] - min[i];
                split_dim = i;
            }
        }

        for (const Photon *photon : photons)
        {
            for (int i = 0; i < 3; i++)
            {
                max[i] = std::max(max[i], photon->position[i]);
                min[i] = std::min(min[i], photon->position[i]);
            }
        }

        std::sort(photons.begin(), photons.end(), [=](Photon *a, Photon *b) { return a->position[split_dim] < b->position[split_dim]; });

        std::vector<Photon *> s1;
        std::vector<Photon *> s2;
        s1.reserve(photons.size() / 2);
        s2.reserve(photons.size() / 2);

        for (size_t i = 0; i < photons.size(); ++i)
        {
            if (i < photons.size() / 2)
            {
                s1.push_back(photons[i]);
            }
            else if (i > photons.size() / 2)
            {
                s2.push_back(photons[i]);
            }
        }

        median = photons[photons.size() / 2];

        if (s1.size() >= 1)
        {
            left = new Node(s1);
        }
        if (s2.size() >= 1)
        {
            right = new Node(s2);
        }
    }
    else
    {
        median = photons[0];
    }
};

void PhotonMap::Node::LocatePhotons(std::vector<Photon *> &nearest, lin_alg::Vector<3> &x, unsigned max, double &r)
{
    if (split_dim >= 0)
    {
        double plane_distance = x[split_dim] - median->position[split_dim];

        if (plane_distance < 0)
        {
            if (left)
            {
                left->LocatePhotons(nearest, x, max, r);
            }
            if (right && std::pow(plane_distance, 2) < std::pow(r, 2))
            {
                right->LocatePhotons(nearest, x, max, r);
            }
        }
        else
        {
            if (right)
            {
                right->LocatePhotons(nearest, x, max, r);
            }
            if (left && std::pow(plane_distance, 2) < std::pow(r, 2))
            {
                left->LocatePhotons(nearest, x, max, r);
            }
        }
    }

    double true_distance = (median->position - x).Magnitude();
    if (true_distance < r)
    {
        nearest.push_back(median);
        std::push_heap(
            nearest.begin(),
            nearest.end(),
            [=](Photon *a, Photon *b) {
                return (a->position - x).Magnitude() < (b->position - x).Magnitude();
            });

        if (nearest.size() > max)
        {
            pop_heap(nearest.begin(), nearest.end(), [&](Photon *a, Photon *b) {
                return (a->position - x).Magnitude() < (b->position - x).Magnitude();
            });
            nearest.pop_back();
            r = (nearest.front()->position - x).Magnitude();
        }
    }
};

std::vector<Photon *> PhotonMap::LocatePhotons(lin_alg::Vector<3> point, unsigned max, double &radius)
{
    std::vector<Photon *> nearest;
    radius = INFINITY;
    root.LocatePhotons(nearest, point, max, radius);
    radius = (nearest.front()->position - point).Magnitude();
    return nearest;
};

void PhotonMap::ScalePhotons(double scalar)
{
    for (auto &photon_ptr : photons)
    {
        photon_ptr->intensity = photon_ptr->intensity.Scale(scalar);
    }
}

lin_alg::Vector<3> PhotonMap::GetIrradianceEsitimate(const RayIntersect &intersect, unsigned N, bool cone_filter, LightingModel *lighting_model, bool &use_shadow_rays)
{
    lin_alg::Vector<3> estimate;

    if (photons.size() == 0)
    {
        return estimate;
    }

    double radius;
    lin_alg::Vector<3> pos = intersect.GetCorrectedPosition();
    std::vector<Photon *> closest_photons = LocatePhotons(pos, N, radius);
    use_shadow_rays = false;

    for (Photon *photon_ptr : closest_photons)
    {
        if (photon_ptr->direction.DotProduct(intersect.normal) < 0.0 && photon_ptr->type == Photon::Indirect)
        {
            lin_alg::Vector<3> photon_radiance = lighting_model->EstimatedPhotonRadiance(*photon_ptr, intersect);
            if (cone_filter)
            {
                double filter = std::max(0.0, 1 - (photon_ptr->position - pos).Magnitude() / radius);
                photon_radiance = photon_radiance * filter;
            }
            estimate += photon_radiance;
        }
        if (photon_ptr->type == Photon::Shadow)
        {
            use_shadow_rays = true;
        }
    }

    estimate = estimate * (1.0 / (M_PI * std::pow(radius, 2.0)));

    if (cone_filter)
    {
        estimate = estimate * 3.0;
    }

    return estimate;
}

void PhotonMap::WriteToFile(const char *filename)
{
    std::_Ios_Openmode openmode = std::ofstream::out | std::ofstream::trunc;
    std::ofstream out(filename, openmode);
    out << std::fixed << std::setprecision(9);
    for (const auto &p_ptr : photons)
    {
        out << "p "
            << p_ptr->type << " "
            << p_ptr->intensity[0] << " "
            << p_ptr->intensity[1] << " "
            << p_ptr->intensity[2] << " "
            << p_ptr->normal[0] << " "
            << p_ptr->normal[1] << " "
            << p_ptr->normal[2] << " "
            << p_ptr->position[0] << " "
            << p_ptr->position[1] << " "
            << p_ptr->position[2] << " "
            << p_ptr->direction[0] << " "
            << p_ptr->direction[1] << " "
            << p_ptr->direction[2] << "\n";
    }

    out.close();
}

PhotonMap::PhotonMap(const char *filename)
{
    std::ifstream in(filename, std::ifstream::in);
    if (!in)
    {
        std::cerr << "Cannot open " << filename << "\n";
        exit(1);
    }

    std::string line;

    while (getline(in, line))
    {
        // check for photon
        if (line.substr(0, 2) == "p ")
        {
            lin_alg::Vector<3> position;
            lin_alg::Vector<3> direction;
            lin_alg::Vector<3> intensity;
            lin_alg::Vector<3> normal;
            unsigned int type;
            
            std::istringstream v(line.substr(2));
            v >> type;
            v >> intensity[0];
            v >> intensity[1];
            v >> intensity[2];
            v >> normal[0];
            v >> normal[1];
            v >> normal[2];
            v >> position[0];
            v >> position[1];
            v >> position[2];
            v >> direction[0];
            v >> direction[1];
            v >> direction[2];
            photons.push_back(new Photon(position, direction, intensity, static_cast<Photon::Type>(type)));
        }
    }

    in.close();

    root = Node(photons);
}