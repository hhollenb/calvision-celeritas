#include "SignalRunAction.hh"

#include <G4HCofThisEvent.hh>
#include <G4Event.hh>

SignalRun::SignalRun()
    : G4Run()
{}

void SignalRun::RecordEvent(G4Event const* event) 
{
    unsigned int event_num = event->GetEventID();

    EventHistograms result;

    G4HCofThisEvent* HCE = event->GetHCofThisEvent();
    for (int i = 0; i < HCE->GetNumberOfCollections(); i++)
    {
        auto const* hc = (SignalHitsCollection*) HCE->GetHC(i);
        result.emplace(hc->GetSDname(), hc->hists());
    }

    event_hists_.emplace(event_num, std::move(result));
}

void SignalRun::Merge(G4Run const* run)
{
    SignalRun const* sig_run = (SignalRun const*) run;
    for (auto const& hist : sig_run->event_hists())
    {
        event_hists_.insert(hist);
    }
}

auto SignalRun::event_hists() const -> std::map<unsigned int, EventHistograms> const&
{
    return event_hists_;
}



SignalRunAction::SignalRunAction(inp::Config const& config, PrimaryGeneratorAction* primary_action)
    : config_(config)
    , primary_action_(primary_action)
{
    if (!primary_action_)
    {
        root_writer_ = std::make_unique<RootSignalWriter>(config_.output.output_filename);
    }
}

G4Run* SignalRunAction::GenerateRun()
{
    return new SignalRun();
}

void SignalRunAction::BeginOfRunAction(G4Run const* run)
{
    if (primary_action_)
    {
        primary_action_->set_energy(config_.primary.energies[run->GetRunID()]);
    }
}

void SignalRunAction::EndOfRunAction(G4Run const* run)
{
    if (this->IsMaster())
    {
        SignalRun const* sig_run = (SignalRun const*) run;
        
        auto run_writer = root_writer_->next_run(config_.primary.energies[run->GetRunID()]);
        for (auto const& [event_id, hists] : sig_run->event_hists())
        {
            run_writer(event_id, hists);
        }
    }
}
