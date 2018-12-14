#ifndef _INCLUDE_MATERIAL_H_
#define _INCLUDE_MATERIAL_H_

#include "photonpathray.h"
#include "rayintersect.h"
#include "utilities.h"
#include <memory>
#include <random>
#include <ctime>

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
        this->k_t = {k_t, k_t, k_t};
        this->refractive_index = refractive_index;
        return *this;
    };

    Material &AddTransparency(lin_alg::Vector<3> k_t, double refractive_index)
    {
        this->k_t = k_t;
        this->refractive_index = refractive_index;
        return *this;
    };

    const lin_alg::Vector<3> &GetRefractionConstant() const
    {
        return k_t;
    };

    const double &GetRefractiveIndex() const
    {
        return refractive_index;
    }

    Material &IsEmitter()
    {
        this->is_emitter = true;
        return *this;
    }

    const bool &IsEmmitter() const
    {
        return this->is_emitter;
    }

#pragma endregion

#pragma region Photon Mapping Methods

    enum PhotonOutcome
    {
        Absorbed,
        Reflected_Diffuse,
        Reflected_Phong_Specular,
        Reflected_Perfect_Specular,
        Transmitted
    };

    Material &IntialiseRussianRoulette()
    {
        reflected_diffuse = this->k_d.Average(); // + k_s;
        reflected_phong_specular = this->k_s;
        reflected_specular = this->k_r;
        transmitted = k_t.Average();
        absorbed = 1 - reflected_diffuse - reflected_specular - k_t.Average() - reflected_phong_specular;
        return *this;
    }

    Material::PhotonOutcome GetPhotonOutcome()
    {
        //std::uniform_real_distribution<> distribution = std::uniform_real_distribution<>(0.0, 1.0);
        double val = Random::Uniform(0.0, 1.0);
        if (val < reflected_diffuse)
        {
            return Reflected_Diffuse;
        }
        else if (val < reflected_diffuse + reflected_phong_specular)
        {
            return Reflected_Phong_Specular;
        }
        else if (val < reflected_diffuse + reflected_specular + reflected_phong_specular)
        {
            return Reflected_Perfect_Specular;
        }
        else if (val < reflected_diffuse + reflected_specular + transmitted + reflected_phong_specular)
        {
            return Transmitted;
        }
        return Absorbed;
    };

    // Photon Mapping probabilities
    double absorbed = 1;
    double reflected_diffuse = 0;
    double reflected_specular = 0;
    double reflected_phong_specular = 0;
    double transmitted = 0;

#pragma endregion

  protected:
    // Diffuse constants
    lin_alg::Vector<3> k_d;

    // Specular constant
    double k_s = 0;

    // Reflection constant
    double k_r = 0;

    // Refraction constant
    lin_alg::Vector<3> k_t;
    double refractive_index = 1;

    bool is_emitter = false;
};

#endif