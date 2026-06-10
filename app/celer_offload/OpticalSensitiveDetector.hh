#pragma once

#include <string>

#include <G4VSensitiveDetector.hh>

#include "OpticalHit.hh"

class SignalSensitiveDetector : public G4VSensitiveDetector
{
  public:
    SignalSensitiveDetector(std::string const& name);

    bool ProcessHits(G4Step* step, G4TouchableHistory*) override;

    void Initialize(G4HCofThisEvent* HCE) override;
    void EndOfEvent(G4HCofThisEvent* HCE) override;

  private:
    int signal_hc_id_{-1};
    SignalHitsCollection* signal_hc_;
};
