#pragma once

#include "G4UserEventAction.hh"

#include "EnergyDepositWriter.hh"
#include "../OpticalHitRecorder.hh"

class EventAction : public G4UserEventAction
{
  public:
    EventAction(EnergyDepositWriter* edep_writer, OpticalHitRecorder* record_hit);

    void BeginOfEventAction(G4Event const* event) override;
    void EndOfEventAction(G4Event const* event) override;

  private:
    EnergyDepositWriter* edep_writer_;
    OpticalHitRecorder* record_hit_;
};
