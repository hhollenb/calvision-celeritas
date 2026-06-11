#include "GeneratorOffload.hh"

#include <G4LogicalVolumeStore.hh>

#include "celeritas/g4/GeantOffloadUtils.hh"
#include "celeritas/optical/gen/GeneratorData.hh"
#include "celeritas/ext/GeantStepView.hh"
#include "celeritas/ext/GeantTrackView.hh"
#include "accel/LocalOpticalGenOffload.hh"
#include "accel/detail/IntegrationSingleton.hh"

#include "Config.hh"


std::string to_string(celeritas::GeneratorType t)
{
    switch (t)
    {
        case celeritas::GeneratorType::cherenkov: return "cherenkov";
        case celeritas::GeneratorType::scintillation: return "scintillation";
    }

    return "nogen";
}

BaseGeneratorOffload::Options BaseGeneratorOffload::Options::from_config(inp::Config config)
{
    BaseGeneratorOffload::Options opts;
    opts.track_celer = config.output.record_celeritas;
    opts.track_g4 = config.output.record_geant4;
    opts.allowed_names = config.detector.allowed_volumes;
    return opts;
}


BaseGeneratorOffload::BaseGeneratorOffload(celeritas::GeneratorType gen_type, Options opts)
    : allowed_names_(opts.allowed_names)
    , track_g4_(opts.track_g4)
    , track_celer_(opts.track_celer)
    , gen_type_(gen_type)
{}

void BaseGeneratorOffload::make_allowed_volumes()
{
    if (allowed_names_)
    {
        auto const* volume_store = G4LogicalVolumeStore::GetInstance();

        allowed_vols_.emplace();
        for (auto const& name : *allowed_names_)
        {
            auto const* vol = volume_store->GetVolume(name, true);
            allowed_vols_->insert(vol);
        }
    }
}

bool BaseGeneratorOffload::is_allowed_volume(G4Step const& step) const
{
    bool result = true;
    if (allowed_vols_)
    {
        auto const* pre_vol = step.GetPreStepPoint()->GetPhysicalVolume();
        auto const* post_vol = step.GetPostStepPoint()->GetPhysicalVolume();

        if (pre_vol)
        {
            result = result && allowed_vols_->count(pre_vol->GetLogicalVolume()) == 1;
        }
        if (post_vol)
        {
            result = result && allowed_vols_->count(post_vol->GetLogicalVolume()) == 1;
        }
        // return (allowed_vols_->count(pre_vol) == 1) && (allowed_vols_->count(post_vol) == 1);
    }
    return result;
}

void BaseGeneratorOffload::offload(G4Track const& track, G4Step const& step, unsigned int num_photons)
{
    // auto pre_pv = step.GetPreStepPoint()->GetPhysicalVolume();
    // auto post_pv = step.GetPostStepPoint()->GetPhysicalVolume();

    // if (pre_pv != post_pv)
    // {
    //     std::cout << "Crossed boundary energy deposit? " << pre_pv->GetName() << " --> " << post_pv->GetName() << " (" << num_photons << " photons lost)\n";
    //     return;
    // }

    if (num_photons > 0 && this->is_allowed_volume(step))
    {
        auto data = celeritas::distribution_from_step(step);
        data.type = gen_type_;
        data.num_photons = static_cast<celeritas::size_type>(num_photons);
        data.continuous_edep_fraction = 1;

        // if (gen_type_ == celeritas::GeneratorType::cherenkov && (data.points[celeritas::StepPoint::pre].speed.value() == 0 || data.points[celeritas::StepPoint::post].speed.value() == 0))
        // {
        //     std::cout << "At rest step point for cherenkov. Losing " << num_photons << " photons\n";
        //     return;
        // }

        // Push generator distribution for this step to offload
        auto& local = celeritas::detail::IntegrationSingleton::instance().local_offload();
        auto* gen_offload = dynamic_cast<celeritas::LocalOpticalGenOffload*>(&local);

        CELER_VALIDATE(gen_offload,
                       << "LocalOpticalGenOffload required for "
                          "GeneratorOffload");

        // CELER_LOG_LOCAL(debug)
        if (data.num_photons > 1000)
        {
            G4cout << "Offloading " << data.num_photons << " " << to_string(gen_type_) << " photons" << G4endl;
        }

        gen_offload->Push(data);
    }
}


CherenkovOffload::CherenkovOffload(BaseGeneratorOffload::Options opts)
    : BaseGeneratorOffload(celeritas::GeneratorType::cherenkov, std::move(opts))
{}

void CherenkovOffload::PreparePhysicsTable(G4ParticleDefinition const& particle)
{
    G4Cerenkov::PreparePhysicsTable(particle);
    this->make_allowed_volumes();
}

G4VParticleChange* CherenkovOffload::PostStepDoIt(G4Track const& track, G4Step const& step)
{
    bool tracking_vol = this->is_allowed_volume(step);
    this->SetStackPhotons(this->track_g4() && tracking_vol);

    auto* result = G4Cerenkov::PostStepDoIt(track, step);

    if (this->track_celer() && tracking_vol && this->GetNumPhotons() > 0)
    {
        this->offload(track, step, this->GetNumPhotons());
    }

    return result;
}



ScintillationOffload::ScintillationOffload(BaseGeneratorOffload::Options opts)
    : BaseGeneratorOffload(celeritas::GeneratorType::scintillation, std::move(opts))
{}

void ScintillationOffload::PreparePhysicsTable(G4ParticleDefinition const& particle)
{
    G4Scintillation::PreparePhysicsTable(particle);
    this->make_allowed_volumes();
}

G4VParticleChange* ScintillationOffload::PostStepDoIt(G4Track const& track, G4Step const& step)
{
    bool tracking_vol = this->is_allowed_volume(step);
    this->SetStackPhotons(this->track_g4() && tracking_vol);

    auto* result = G4Scintillation::PostStepDoIt(track, step);

    if (this->track_celer() && tracking_vol && this->GetNumPhotons() > 0)
    {
        this->offload(track, step, this->GetNumPhotons());
    }

    return result;
}

G4VParticleChange* ScintillationOffload::AtRestDoIt(G4Track const& track, G4Step const& step)
{
    return this->PostStepDoIt(track, step);
}
