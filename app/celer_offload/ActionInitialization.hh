#pragma once

#include <G4VUserActionInitialization.hh>

#include "Config.hh"

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(inp::Config const& config);

    void BuildForMaster() const override;
    void Build() const override;

  private:
    inp::Config config_;
};
