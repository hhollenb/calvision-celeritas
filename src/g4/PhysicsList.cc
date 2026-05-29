#include "PhysicsList.hh"

#include "celeritas/g4/SupportedOpticalPhysics.hh"

PhysicsList::PhysicsList(EnergyDepositWriter* edep_writer)
    : FTFP_BERT(0)
    , edep_writer_(edep_writer)
{
    offload_options_.allowed_names = {
        "crystal",
        "resin_window",
        "silicone_gap",
        "air_gap",
        "fsc_crystal",
        "fsc_silicone_gap",
        "fsc_resin_window",
        "fsc_air_gap",
    };
    offload_options_.edep_writer = edep_writer_;

    this->RegisterPhysics(new celeritas::SupportedOpticalPhysics(this->physics_options()));
}

celeritas::GeantOpticalPhysicsOptions PhysicsList::optical_options() const
{
    celeritas::GeantOpticalPhysicsOptions optical;

    optical.cherenkov.emplace();
    optical.cherenkov->custom_cherenkov = [this]() {
            return std::make_unique<CherenkovOffload>(offload_options_);
        };
    optical.cherenkov->stack_photons = true;

    // optical.scintillation = std::nullopt;
    optical.scintillation.emplace();
    optical.scintillation->custom_scintillation = [this]() {
            return std::make_unique<ScintillationOffload>(offload_options_);
        };
    optical.scintillation->stack_photons = true;

    optical.boundary->invoke_sd = true;
    optical.absorption = true;
    optical.rayleigh_scattering = true;

    return optical;
}

celeritas::GeantPhysicsOptions PhysicsList::physics_options() const
{
    celeritas::GeantPhysicsOptions opts;
    opts.optical = this->optical_options();
    return opts;
}
