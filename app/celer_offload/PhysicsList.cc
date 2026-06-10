#include "PhysicsList.hh"

#include "accel/gen/CherenkovOffload.hh"
#include "accel/gen/ScintillationOffload.hh"
#include "celeritas/g4/SupportedOpticalPhysics.hh"

PhysicsList::PhysicsList(std::unordered_set<std::string> const& allowed_vols)
    : FTFP_BERT(0)
    , allowed_vols_(allowed_vols)
{
    this->RegisterPhysics(new celeritas::SupportedOpticalPhysics(this->physics_options()));
}

celeritas::GeantOpticalPhysicsOptions PhysicsList::optical_options() const
{
    celeritas::GeantOpticalPhysicsOptions optical;

    optical.cherenkov.emplace();
    // optical.cherenkov->custom_cherenkov = [this]() {
    //         return std::make_unique<celeritas::CherenkovOffload>(allowed_vols_);
    //     };

    // optical.scintillation = std::nullopt;
    optical.scintillation.emplace();
    // optical.scintillation->custom_scintillation = [this]() {
    //         return std::make_unique<celeritas::ScintillationOffload>(allowed_vols_);
    //     };

    // optical.cherenkov->stack_photons = false;
    // optical.scintillation->stack_photons = false;

    optical.cherenkov->stack_photons = true;
    optical.scintillation->stack_photons = true;

    optical.boundary->invoke_sd = true;
    optical.absorption = true;
    optical.rayleigh_scattering = false;

    return optical;
}

celeritas::GeantPhysicsOptions PhysicsList::physics_options() const
{
    celeritas::GeantPhysicsOptions opts;
    opts.optical = this->optical_options();
    return opts;
}
