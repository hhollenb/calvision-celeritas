#include "OpticalSensitiveDetector.hh"

#include <G4SystemOfUnits.hh>
#include <G4OpticalPhoton.hh>
#include <G4VProcess.hh>
#include <G4Scintillation.hh>
#include <G4Cerenkov.hh>

#include "celeritas/optical/Types.hh"

OpticalSensitiveDetector::OpticalSensitiveDetector(G4String sd_name, OpticalHitRecorder* record_hit, DetectorMapping* detector_mapping)
    : G4VSensitiveDetector(sd_name)
    , record_hit_(record_hit)
    , detector_mapping_(detector_mapping)
{}

G4bool OpticalSensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    if (step->GetTrack()->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition())
    {
        auto const* ps = step->GetPreStepPoint();
        auto const* history = ps->GetTouchable();

        OpticalHit hit;
        hit.energy = ps->GetKineticEnergy() / CLHEP::eV;
        hit.time = ps->GetGlobalTime() / CLHEP::ns;
        hit.detector_id = (*detector_mapping_)(history);

        auto const* proc = step->GetTrack()->GetCreatorProcess();
        if (dynamic_cast<G4Scintillation const*>(proc))
        {
            hit.gen_type = celeritas::GeneratorType::scintillation;
        }
        else if (dynamic_cast<G4Cerenkov const*>(proc))
        {
            hit.gen_type = celeritas::GeneratorType::cherenkov;
        }

        if (hit.detector_id != static_cast<unsigned int>(-1))
        {
            (*record_hit_)(hit);
        }

        step->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
    }
    return false;
}
