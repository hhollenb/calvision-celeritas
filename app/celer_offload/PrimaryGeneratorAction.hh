#pragma once

#include <memory>

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleGun.hh>

#include "Config.hh"

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(inp::Primary const& primary);

    void GeneratePrimaries(G4Event* event) override;

    void set_energy(double energy);

  private:
    std::shared_ptr<G4ParticleGun> particle_gun_;
    inp::Primary primary_;
};
