#pragma once

#include <G4RunManager.hh>
#include <G4RunManagerFactory.hh>

#include "geocel/GeantGeoParams.hh"

#include "../DetectorMapping.hh"
#include "../OpticalHitRecorder.hh"
#include "EnergyDepositWriter.hh"

class GeantRunner
{
  public:
    GeantRunner();

    void operator()();

  private:
    std::unique_ptr<G4RunManager> run_manager_;
    std::unique_ptr<EnergyDepositWriter> edep_writer_;
    std::unique_ptr<OpticalHitRecorder> hit_recorder_;
    std::shared_ptr<celeritas::GeantGeoParams> geo_params_;
    std::unique_ptr<DetectorMapping> detector_mapping_;
};
