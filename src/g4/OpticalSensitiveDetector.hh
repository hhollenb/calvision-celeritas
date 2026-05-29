#pragma once

#include <G4VSensitiveDetector.hh>

#include "../DetectorMapping.hh"
#include "../OpticalHitRecorder.hh"

class OpticalSensitiveDetector : public G4VSensitiveDetector
{
  public:
    OpticalSensitiveDetector(G4String sd_name, OpticalHitRecorder* record_hit, DetectorMapping* detector_mapping);

    void Initialize(G4HCofThisEvent*) final {}
    void EndOfEvent(G4HCofThisEvent*) final {}
    G4bool ProcessHits(G4Step* step, G4TouchableHistory*) override;

  private:
    OpticalHitRecorder* record_hit_;
    DetectorMapping* detector_mapping_;
};
