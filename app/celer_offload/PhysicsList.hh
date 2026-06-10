#pragma once

#include <unordered_set>
#include <string>

#include <FTFP_BERT.hh>

#include "celeritas/g4/SupportedOpticalPhysics.hh"

class PhysicsList : public FTFP_BERT
{
  public:
    PhysicsList(std::unordered_set<std::string> const& allowed_vols);

    celeritas::GeantOpticalPhysicsOptions optical_options() const;
    celeritas::GeantPhysicsOptions physics_options() const;

  private:
    std::unordered_set<std::string> allowed_vols_;
};
