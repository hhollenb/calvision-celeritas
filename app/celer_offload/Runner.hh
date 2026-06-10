#pragma once

#include <G4RunManager.hh>

#include "Config.hh"

class Runner
{
  public:
    Runner(inp::Config config);

    void operator()();

  private:
    std::unique_ptr<G4RunManager> run_manager_;
    inp::Config config_;
};
