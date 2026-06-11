#pragma once

#include <string>

#include <G4VSensitiveDetector.hh>

#include "corecel/cont/Span.hh"
#include "celeritas/optical/DetectorData.hh"

#include "OpticalHit.hh"

namespace inp
{
class Config;
} // namespace inp

class OpticalHitRecorder
{
  public:
    using CeleritasSpanHits = celeritas::Span<celeritas::optical::DetectorHit const>;

    OpticalHitRecorder();

    unsigned int register_detector(std::string const& sd_name);

    void initialize(unsigned int detector_id, SignalHitsCollection* celer_signal_hc, SignalHitsCollection* g4_signal_hc);

    void operator()(unsigned int detector_id, G4Step* step);
    void operator()(CeleritasSpanHits hits);

  private:
    std::map<std::string, unsigned int> g4_sd_mapping_;
    std::vector<SignalHitsCollection*> g4_signal_hc_;
    std::vector<SignalHitsCollection*> celer_signal_hc_;
};

class SignalSensitiveDetector : public G4VSensitiveDetector
{
  public:
    SignalSensitiveDetector(std::string const& name, OpticalHitRecorder* hit_recorder, inp::Config const& config);

    bool ProcessHits(G4Step* step, G4TouchableHistory*) override;

    void Initialize(G4HCofThisEvent* HCE) override;
    void EndOfEvent(G4HCofThisEvent* HCE) override;

  private:
    int celer_signal_hc_id_{-1};
    int g4_signal_hc_id_{-1};
    SignalHitsCollection* celer_signal_hc_;
    SignalHitsCollection* g4_signal_hc_;

    unsigned int detector_id_;

    OpticalHitRecorder* hit_recorder_;

    bool record_geant4_;
    bool record_celeritas_;
};
