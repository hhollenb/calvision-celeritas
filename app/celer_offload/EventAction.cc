#include "EventAction.hh"

#include <accel/UserActionIntegration.hh>

void EventAction::BeginOfEventAction(G4Event const* event)
{
    celeritas::UserActionIntegration::Instance().BeginOfEventAction(event);
}

void EventAction::EndOfEventAction(G4Event const* event)
{
    celeritas::UserActionIntegration::Instance().EndOfEventAction(event);
}
