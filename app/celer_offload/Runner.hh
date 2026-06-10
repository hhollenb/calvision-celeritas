#pragma once

#include <memory>

#include <G4RunManager.hh>

#include "Config.hh"
#include "OpticalSensitiveDetector.hh"

class Runner
{
  public:
    Runner(inp::Config config);

    void operator()();

  private:
    std::unique_ptr<G4RunManager> run_manager_;
    inp::Config config_;
    std::unique_ptr<OpticalHitRecorder> hit_recorder_;
};
