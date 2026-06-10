#pragma once

#include <string>

#include <G4VSensitiveDetector.hh>

#include "corecel/cont/Span.hh"
#include "celeritas/optical/DetectorData.hh"

#include "OpticalHit.hh"

class OpticalHitRecorder
{
  public:
    using CeleritasSpanHits = celeritas::Span<celeritas::optical::DetectorHit const>;

    OpticalHitRecorder();

    void register_detector(std::string const& sd_name);

    void initialize(std::string const& sd_name, SignalHitsCollection* celer_signal_hc, SignalHitsCollection* g4_signal_hc);

    void operator()(std::string const& sd_name, G4Step* step);
    void operator()(CeleritasSpanHits hits);

  private:
    std::map<std::string, unsigned int> g4_sd_mapping_;
    std::vector<SignalHitsCollection*> g4_signal_hc_;
    std::vector<SignalHitsCollection*> celer_signal_hc_;
};

class SignalSensitiveDetector : public G4VSensitiveDetector
{
  public:
    SignalSensitiveDetector(std::string const& name, OpticalHitRecorder* hit_recorder);

    bool ProcessHits(G4Step* step, G4TouchableHistory*) override;

    void Initialize(G4HCofThisEvent* HCE) override;
    void EndOfEvent(G4HCofThisEvent* HCE) override;

  private:
    int celer_signal_hc_id_{-1};
    int g4_signal_hc_id_{-1};
    SignalHitsCollection* celer_signal_hc_;
    SignalHitsCollection* g4_signal_hc_;

    OpticalHitRecorder* hit_recorder_;
};
