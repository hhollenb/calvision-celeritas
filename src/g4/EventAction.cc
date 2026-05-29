#include "EventAction.hh"

#include <iostream>

#include <G4Event.hh>



EventAction::EventAction(EnergyDepositWriter* edep_writer, OpticalHitRecorder* record_hit)
    : G4UserEventAction()
    , edep_writer_(edep_writer)
    , record_hit_(record_hit)
{
}

void EventAction::BeginOfEventAction(G4Event const* event)
{
    edep_writer_->open("edep_" + std::to_string(event->GetEventID()) + ".root");
    record_hit_->start_event(event->GetEventID());
}

void EventAction::EndOfEventAction(G4Event const* event)
{
    edep_writer_->close();
    record_hit_->end_event();
}
