#pragma once

#include <FTFP_BERT.hh>

#include "celeritas/g4/SupportedOpticalPhysics.hh"

#include "GeneratorOffload.hh"
#include "EnergyDepositWriter.hh"



class PhysicsList : public FTFP_BERT
{
  public:
    PhysicsList(EnergyDepositWriter* edep_writer);

    celeritas::GeantOpticalPhysicsOptions optical_options() const;
    celeritas::GeantPhysicsOptions physics_options() const;

  private:
    BaseGeneratorOffload::Options offload_options_;
    EnergyDepositWriter* edep_writer_;
};
