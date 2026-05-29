#pragma once

#include "celeritas/optical/Runner.hh"

#include "../DetectorMapping.hh"
#include "../OpticalHitRecorder.hh"
#include "EnergyDepositReader.hh"

class CelerRunner
{
  public:
    CelerRunner();

    void operator()();

  private:

    void run_event(unsigned int event);

    std::unique_ptr<celeritas::optical::Runner> runner_;
    std::unique_ptr<OpticalHitRecorder> hit_recorder_;
    std::unique_ptr<EnergyDepositReader> edep_reader_;
    std::unique_ptr<DetectorMapping> detector_mapping_;
};
