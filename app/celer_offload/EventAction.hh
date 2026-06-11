#pragma once

#include <chrono>

#include "G4UserEventAction.hh"

class EventAction : public G4UserEventAction
{
  public:
    EventAction(bool celer_offload);

    void BeginOfEventAction(G4Event const*) override;
    void EndOfEventAction(G4Event const*) override;

  private:
    bool celer_offload_;

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint start_time_;
};
