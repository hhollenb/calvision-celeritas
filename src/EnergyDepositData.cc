#include "EnergyDepositData.hh"

StepData& StepData::operator=(celeritas::optical::GeneratorStepData const& data)
{
    speed = celeritas::value_as<celeritas::units::LightSpeed>(data.speed);
    time = data.time;
    pos_x = data.pos[0];
    pos_y = data.pos[1];
    pos_z = data.pos[2];
    return *this;
}

celeritas::optical::GeneratorStepData StepData::to_celer() const
{
    celeritas::optical::GeneratorStepData result;
    result.speed = celeritas::units::LightSpeed{speed};
    result.time = time;
    result.pos[0] = pos_x;
    result.pos[1] = pos_y;
    result.pos[2] = pos_z;
    return result;
}

EdepData& EdepData::operator=(celeritas::optical::GeneratorDistributionData const& data)
{
    type = static_cast<unsigned int>(data.type);
    num_photons = data.num_photons;
    primary = data.primary.unchecked_get();
    step_length = data.step_length;
    charge = celeritas::value_as<celeritas::units::ElementaryCharge>(data.charge);
    material = data.material.unchecked_get();
    continuous_edep_fraction = data.continuous_edep_fraction;
    pre_step = data.points[celeritas::StepPoint::pre];
    post_step = data.points[celeritas::StepPoint::post];
    return *this;
}

celeritas::optical::GeneratorDistributionData EdepData::to_celer() const
{
    celeritas::optical::GeneratorDistributionData result;
    result.type = static_cast<celeritas::GeneratorType>(type);
    result.num_photons = num_photons;
    result.primary = celeritas::id_cast<celeritas::PrimaryId>(primary);
    result.step_length = step_length;
    result.charge = celeritas::units::ElementaryCharge{charge};
    result.material = celeritas::id_cast<celeritas::OptMatId>(material);
    result.continuous_edep_fraction = continuous_edep_fraction;
    result.points[celeritas::StepPoint::pre] = pre_step.to_celer();
    result.points[celeritas::StepPoint::post] = post_step.to_celer();
    return result;
}
