#include "PhysicsList.hh"

// #include "accel/gen/CherenkovOffload.hh"
// #include "accel/gen/ScintillationOffload.hh"

PhysicsList::PhysicsList(std::vector<std::string> const& allowed_vols)
    : FTFP_BERT(0)
{
    this->RegisterPhysics(new celeritas::SupportedOpticalPhysics(this->physics_options()));

    gen_opts_.track_photons = true;
    gen_opts_.allowed_names = allowed_vols;
}

celeritas::GeantOpticalPhysicsOptions PhysicsList::optical_options() const
{
    celeritas::GeantOpticalPhysicsOptions optical;

    optical.cherenkov.emplace();
    optical.cherenkov->custom_cherenkov = [this]() {
            return std::make_unique<CherenkovOffload>(gen_opts_);
        };

    // optical.scintillation = std::nullopt;
    optical.scintillation.emplace();
    optical.scintillation->custom_scintillation = [this]() {
            return std::make_unique<ScintillationOffload>(gen_opts_);
        };

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
