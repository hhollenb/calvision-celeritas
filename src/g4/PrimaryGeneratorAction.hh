#pragma once

#include <memory>

#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleGun.hh>

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();

    void GeneratePrimaries(G4Event* event) override;

  private:
    std::shared_ptr<G4ParticleGun> particle_gun_;
};
