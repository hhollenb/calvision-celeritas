#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4PionZero.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction()
{
    particle_gun_ = std::make_shared<G4ParticleGun>(1);

    particle_gun_->SetParticleDefinition(G4Electron::Definition());
    // particle_gun_->SetParticleDefinition(G4PionZero::Definition());
    particle_gun_->SetParticleEnergy(5 * GeV);
    particle_gun_->SetParticlePosition(G4ThreeVector{0, 3.5, -50} * cm);
    particle_gun_->SetParticleMomentumDirection(G4ThreeVector{0, 0, 1}.unit());
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    particle_gun_->GeneratePrimaryVertex(event);
}
