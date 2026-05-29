#pragma once

#include "celeritas/optical/gen/GeneratorData.hh"

#include <Math/Vector3D.h>

struct StepData
{
    using Vec3 = ROOT::Math::XYZVector;

    double speed;
    double time;
    double pos_x;
    double pos_y;
    double pos_z;

    StepData& operator=(celeritas::optical::GeneratorStepData const&);
    celeritas::optical::GeneratorStepData to_celer() const;
};

struct EdepData
{
    unsigned int type;
    unsigned int num_photons;
    unsigned int primary;
    double step_length;
    double charge;
    unsigned int material;
    double continuous_edep_fraction;
    StepData pre_step;
    StepData post_step;

    EdepData& operator=(celeritas::optical::GeneratorDistributionData const&);
    celeritas::optical::GeneratorDistributionData to_celer() const;
};
