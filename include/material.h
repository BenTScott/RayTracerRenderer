#ifndef _INCLUDE_MATERIAL_H_
#define _INCLUDE_MATERIAL_H_

#include "photonpathray.h"
#include "rayintersect.h"
#include <memory>
#include <random>

class Material
{
  public:
    Material(){};
    Material(lin_alg::Vector<3> k_d, double k_s) : k_d(k_d), k_s(k_s){};
    Material(lin_alg::Vector<3> k_d) : k_d(k_d){};

#pragma region Accessors
    Material &SetDiffuseConstant(lin_alg::Vector<3> k_d)
    {
        this->k_d = k_d;
        return *this;
    };

    const lin_alg::Vector<3> GetDiffuseConstant() const
    {
        return k_d;
    };

    Material &SetSpecularConstant(double k_s)
    {
        this->k_s = k_s;
        return *this;
    };

    const double &GetSpecularConstant() const
    {
        return k_s;
    };

    Material &AddReflection(double k_r)
    {
        this->k_r = k_r;
        return *this;
    };

    const double &GetReflectionConstant() const
    {
        return k_r;
    };

    Material &AddTransparency(double k_t, double refractive_index)
    {
        this->k_t = k_t;
        this->refractive_index = refractive_index;
        return *this;
    };

    const double &GetRefractionConstant() const
    {
        return k_t;
    };

    const double &GetRefractiveIndex() const
    {
        return refractive_index;
    }

#pragma endregion

#pragma region Photon Mapping Methods

    enum PhotonOutcome
    {
        Absorbed,
        Reflected,
        Transmitted
    };

    enum TransmissionOutcome
    {
        Refracted,
        MirrorReflection
    };

    Material &IntialiseRussianRoulette(double absorbed, double reflected)
    {
        assert(absorbed >= 0 && reflected >= 0 && absorbed + reflected <= 1);
        this->absorbed = absorbed;
        this->reflected = reflected;
        return *this;
    }

    Material &InitialiseTransmission(double transmitted_refracted)
    {
        assert(transmitted_refracted >= 0 and transmitted_refracted <= 1);
        this->transmitted_refracted = transmitted_refracted;
        return *this;
    }

    Material::PhotonOutcome GetPhotonOutcome()
    {
        double val = distribution(generator);
        if (val < absorbed)
        {
            return Absorbed;
        }
        else if (val < absorbed + reflected)
        {
            return Reflected;
        }

        return Transmitted;
    };

    Material::TransmissionOutcome GetTransmissionOutcome()
    {
        double val = distribution(generator);
        if (val < transmitted_refracted)
        {
            return Refracted;
        }
        else
        {
            return MirrorReflection;
        }
    };

#pragma endregion

  protected:
    // Diffuse constants
    lin_alg::Vector<3> k_d = lin_alg::Vector<3>({1, 1, 1});

    // Specular constant
    double k_s = 0;

    //TODO: Allowed coloured glass and mirrors
    // Reflection constant
    double k_r = 0;

    // Refraction constant
    double k_t = 0;
    double refractive_index = 1;

    // Photon Mapping members
    double absorbed = 1;
    double reflected = 0;
    double transmitted_refracted = 1;
    std::mt19937 generator = std::mt19937(rand());
    std::uniform_real_distribution<> distribution = std::uniform_real_distribution<>(0.0, 1.0);
};

#endif