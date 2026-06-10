#pragma once

#include "G4UserEventAction.hh"

class EventAction : public G4UserEventAction
{
  public:
    void BeginOfEventAction(G4Event const*) override;
    void EndOfEventAction(G4Event const*) override;
};
