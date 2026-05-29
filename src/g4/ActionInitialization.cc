#include "ActionInitialization.hh"

#include "EventAction.hh"
#include "PrimaryGeneratorAction.hh"

ActionInitialization::ActionInitialization(EnergyDepositWriter* edep_writer, OpticalHitRecorder* hit_recorder)
    : G4VUserActionInitialization()
    , edep_writer_(edep_writer)
    , hit_recorder_(hit_recorder)
{}

void ActionInitialization::BuildForMaster() const
{
}

void ActionInitialization::Build() const
{
    this->SetUserAction(new PrimaryGeneratorAction());
    this->SetUserAction(new EventAction(edep_writer_, hit_recorder_));
}
