#include "SignalRunAction.hh"

#include <G4HCofThisEvent.hh>
#include <G4Event.hh>
#include <CeleritasG4.hh>

#include "LeakageSensitiveDetector.hh"



SignalRun::SignalRun()
    : G4Run()
{}

void SignalRun::RecordEvent(G4Event const* event) 
{
    unsigned int event_num = event->GetEventID();

    EventHistograms result;
    ShowerLeakageEnergy leakage_energy;

    G4HCofThisEvent* HCE = event->GetHCofThisEvent();
    for (int i = 0; i < HCE->GetNumberOfCollections(); i++)
    {
        if (auto const* hc = dynamic_cast<ShowerHitsCollection const*>(HCE->GetHC(i)))
        {
            if (hc->GetSDname() == "lateral_leakage")
            {
                leakage_energy.lateral += hc->total_energy();
                leakage_energy.lateral_count += hc->num_particles();
            }
            else if (hc->GetSDname() == "longitudinal_leakage")
            {
                leakage_energy.longitudinal += hc->total_energy();
                leakage_energy.longitudinal_count += hc->num_particles();
            }
        }
        else if (auto const* hc = dynamic_cast<SignalHitsCollection const*>(HCE->GetHC(i)))
        {
            result.emplace(hc->id_name(), hc->hists());
        }
    }

    event_hists_.emplace(event_num, std::move(result));
    event_leakage_.emplace(event_num, std::move(leakage_energy));
}

void SignalRun::Merge(G4Run const* run)
{
    G4cout << "Begin merge" << G4endl;

    SignalRun const* sig_run = (SignalRun const*) run;
    for (auto const& hist : sig_run->event_hists())
    {
        event_hists_.insert(hist);
    }

    for (auto const& leakage : sig_run->event_leakage())
    {
        event_leakage_.insert(leakage);
    }

    G4cout << "End merge" << G4endl;
}

auto SignalRun::event_hists() const -> std::map<unsigned int, EventHistograms> const&
{
    return event_hists_;
}

auto SignalRun::event_leakage() const -> std::map<unsigned int, ShowerLeakageEnergy> const&
{
    return event_leakage_;
}



SignalRunAction::SignalRunAction(inp::Config const& config, PrimaryGeneratorAction* primary_action)
    : config_(config)
    , primary_action_(primary_action)
{}

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

    if (config_.output.record_celeritas)
    {
        // celeritas::TrackingManagerIntegration::Instance().BeginOfRunAction(run);
        celeritas::UserActionIntegration::Instance().BeginOfRunAction(run);
    }
}

void SignalRunAction::EndOfRunAction(G4Run const* run)
{
    if (config_.output.record_celeritas)
    {
        // celeritas::TrackingManagerIntegration::Instance().EndOfRunAction(run);
        celeritas::UserActionIntegration::Instance().EndOfRunAction(run);
    }

    if (this->IsMaster())
    {
        if (!root_writer_)
        {
            root_writer_ = std::make_unique<RootSignalWriter>(config_.output.output_filename);
        }

        SignalRun const* sig_run = (SignalRun const*) run;

        double const primary_energy = config_.primary.energies[run->GetRunID()];
        
        auto run_writer = root_writer_->next_run(primary_energy);
        for (auto const& [event_id, hists] : sig_run->event_hists())
        {
            ShowerLeakageEnergy leakage = sig_run->event_leakage().at(event_id);
            leakage.lateral /= primary_energy;
            leakage.longitudinal /= primary_energy;

            run_writer(event_id, hists, leakage);
        }
    }
}
