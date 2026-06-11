#pragma once

#include <vector>
#include <string>

#include <FTFP_BERT.hh>

#include "GeneratorOffload.hh"
#include "celeritas/g4/SupportedOpticalPhysics.hh"

class PhysicsList : public FTFP_BERT
{
  public:
    PhysicsList(BaseGeneratorOffload::Options gen_opts);

    celeritas::GeantOpticalPhysicsOptions optical_options() const;
    celeritas::GeantPhysicsOptions physics_options() const;

  private:
     BaseGeneratorOffload::Options gen_opts_;
};
