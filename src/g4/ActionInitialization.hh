#pragma once

#include "G4VUserActionInitialization.hh"

#include "EnergyDepositWriter.hh"
#include "../OpticalHitRecorder.hh"


class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(EnergyDepositWriter* edep_writer, OpticalHitRecorder* hit_recorder);

    void BuildForMaster() const override;
    void Build() const override;

  private:
    EnergyDepositWriter* edep_writer_;
    OpticalHitRecorder* hit_recorder_;
};
