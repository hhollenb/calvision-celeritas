#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction(inp::Primary const& primary)
    : G4VUserPrimaryGeneratorAction()
    , primary_(primary)
{
    particle_gun_ = std::make_shared<G4ParticleGun>(1);

    particle_gun_->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(primary.pdg));
    particle_gun_->SetParticlePosition(primary_.position * cm);
    particle_gun_->SetParticleMomentumDirection(primary_.direction.unit());
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    particle_gun_->GeneratePrimaryVertex(event);
}

void PrimaryGeneratorAction::set_energy(double energy)
{
    particle_gun_->SetParticleEnergy(energy * GeV);
}
