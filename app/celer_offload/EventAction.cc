#include "EventAction.hh"

#include <G4Event.hh>

#include <accel/UserActionIntegration.hh>

EventAction::EventAction(bool celer_offload)
    : celer_offload_(celer_offload)
{}

void EventAction::BeginOfEventAction(G4Event const* event)
{
    G4cout << "Beginning event " << event->GetEventID() << G4endl;

    start_time_ = Clock::now();

    if (celer_offload_)
    {
        celeritas::UserActionIntegration::Instance().BeginOfEventAction(event);
    }
}

void EventAction::EndOfEventAction(G4Event const* event)
{
    G4cout << "Ending event " << event->GetEventID() << G4endl;
    if (celer_offload_)
    {
        celeritas::UserActionIntegration::Instance().EndOfEventAction(event);
    }

    TimePoint stop_time = Clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::seconds>(stop_time - start_time_).count();

    G4cout << "Duration: " << elapsed << " seconds" << G4endl;
}
