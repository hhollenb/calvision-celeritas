#include "PhysicsList.hh"

#include <CeleritasG4.hh>

// #include "accel/gen/CherenkovOffload.hh"
#include "accel/gen/ScintillationOffload.hh"

PhysicsList::PhysicsList(BaseGeneratorOffload::Options gen_opts)
    : FTFP_BERT(0)
    , gen_opts_(std::move(gen_opts))
{
    this->RegisterPhysics(new celeritas::SupportedOpticalPhysics(this->physics_options()));
    // this->RegisterPhysics(new celeritas::TrackingManagerConstructor{&celeritas::TrackingManagerIntegration::Instance()});
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
    // optical.scintillation->custom_scintillation = [this]() {
    //         return std::make_unique<celeritas::ScintillationOffload>();
    //     };

    optical.cherenkov->stack_photons = false;
    optical.scintillation->stack_photons = false;

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
