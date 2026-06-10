#include "OpticalSensitiveDetector.hh"

#include <G4OpticalPhoton.hh>
#include <G4VProcess.hh>
#include <G4Scintillation.hh>
#include <G4Cerenkov.hh>

/*
OpticalSensitiveDetector::OpticalSensitiveDetector(std::string const& name)
    : G4VSensitiveDetector(name)
{
    collectionName.insert("OpticalHitsCollection");
}

void OpticalSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
    if (hc_id_ < 0)
    {
        hc_id_ = this->GetCollectionID(0);
    }

    hc_ = new OpticalHitsCollection(this->GetName(), "OpticalHitsCollection");
    HCE->AddHitsCollection(hc_id_, hc_);
}

void OpticalSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
}

bool OpticalSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    if (step->GetTrack()->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        auto const* proc = step->GetTrack()->GetCreatorProcess();
        if (dynamic_cast<G4Scintillation const*>(proc))
        {
            ++hc_->scores().num_scintillation;
        }
        else if (dynamic_cast<G4Cerenkov const*>(proc))
        {
            ++hc_->scores().num_cherenkov;
        }

        step->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
    }
    return false;

}
*/


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


SignalSensitiveDetector::SignalSensitiveDetector(std::string const& name)
    : G4VSensitiveDetector(name)
{
    collectionName.insert("SignalHitsCollection");
}

void SignalSensitiveDetector::Initialize(G4HCofThisEvent* HCE)
{
    if (signal_hc_id_ < 0)
    {
        signal_hc_id_ = this->GetCollectionID(0);
    }

    signal_hc_ = new SignalHitsCollection(this->GetName(), "SignalHitsCollection");
    HCE->AddHitsCollection(signal_hc_id_, signal_hc_);
}

void SignalSensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
}

bool SignalSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    if (step->GetTrack()->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        auto const* ps = step->GetPreStepPoint();

        OpticalHit hit;
        hit.energy = ps->GetKineticEnergy() / CLHEP::eV;
        hit.time = ps->GetGlobalTime() / CLHEP::ns;
        hit.gen_type = get_gen_type(step);

        signal_hc_->score(hit);

        step->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
    }
    return false;
}
