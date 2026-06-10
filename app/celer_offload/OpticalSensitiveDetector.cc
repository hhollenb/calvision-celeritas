#include "OpticalSensitiveDetector.hh"

#include <G4OpticalPhoton.hh>
#include <G4VProcess.hh>
#include <G4Scintillation.hh>
#include <G4Cerenkov.hh>

celeritas::GeneratorType get_gen_type(G4Step const* step)
{
    auto const* proc = step->GetTrack()->GetCreatorProcess();
    if (dynamic_cast<G4Scintillation const*>(proc))
    {
        return celeritas::GeneratorType::scintillation;
    }
    else if (dynamic_cast<G4Cerenkov const*>(proc))
    {
        return celeritas::GeneratorType::cherenkov;
    }

    return celeritas::GeneratorType::size_;
}



OpticalHitRecorder::OpticalHitRecorder()
{}

void OpticalHitRecorder::register_detector(std::string const& sd_name)
{
    g4_sd_mapping_.emplace(sd_name, g4_sd_mapping_.size());
    g4_signal_hc_.push_back(nullptr);
    celer_signal_hc_.push_back(nullptr);

    G4cout << "Registered detector " << sd_name << " with detector ID " << g4_sd_mapping_.at(sd_name) << G4endl;
}

void OpticalHitRecorder::initialize(std::string const& sd_name, SignalHitsCollection* celer_signal_hc, SignalHitsCollection* g4_signal_hc)
{
    unsigned int det_id = g4_sd_mapping_.at(sd_name);
    celer_signal_hc_[det_id] = celer_signal_hc;
    g4_signal_hc_[det_id] = g4_signal_hc;
}

void OpticalHitRecorder::operator()(std::string const& sd_name, G4Step* step)
{
    if (step->GetTrack()->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        if (auto* g4_signal_hc = g4_signal_hc_[g4_sd_mapping_.at(sd_name)])
        {
           auto const* ps = step->GetPreStepPoint();

            OpticalHit hit;
            hit.energy = ps->GetKineticEnergy() / CLHEP::eV;
            hit.time = ps->GetGlobalTime() / CLHEP::ns;
            hit.gen_type = get_gen_type(step);

            g4_signal_hc->score(hit);
        }

        step->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
    }
}

void OpticalHitRecorder::operator()(CeleritasSpanHits hits)
{
    for (auto const& hit : hits)
    {
        OpticalHit h;
        h.energy = celeritas::value_as<celeritas::units::MevEnergy>(hit.energy) * 1e6;
        h.time = hit.time / celeritas::units::nanosecond.value();
        h.gen_type = hit.gen_type;

        celer_signal_hc_[hit.detector.get()]->score(h);
    }
}




SignalSensitiveDetector::SignalSensitiveDetector(std::string const& name, OpticalHitRecorder* hit_recorder)
    : G4VSensitiveDetector(name)
    , hit_recorder_(hit_recorder)
{
    collectionName.insert("CeleritasSignalHitsCollection");
    collectionName.insert("Geant4SignalHitsCollection");

    hit_recorder_->register_detector(name);
}

void SignalSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
    if (celer_signal_hc_id_ < 0)
    {
        celer_signal_hc_id_ = this->GetCollectionID(0);
    }

    if (g4_signal_hc_id_ < 0)
    {
        g4_signal_hc_id_ = this->GetCollectionID(1);
    }

    celer_signal_hc_ = new SignalHitsCollection(this->GetName(), collectionName[0]);
    g4_signal_hc_ = new SignalHitsCollection(this->GetName(), collectionName[1]);
    HCE->AddHitsCollection(celer_signal_hc_id_, celer_signal_hc_);
    HCE->AddHitsCollection(g4_signal_hc_id_, g4_signal_hc_);

    hit_recorder_->initialize(this->GetName(), celer_signal_hc_, g4_signal_hc_);
}

void SignalSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
}

bool SignalSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    (*hit_recorder_)(this->GetName(), step);
    return false;
}
