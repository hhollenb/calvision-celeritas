#include "RunAction.hh"

#include <G4HCofThisEvent.hh>
#include <G4Event.hh>

OpticalRun::OpticalRun()
    : G4Run()
{}

void OpticalRun::RecordEvent(G4Event const* event)
{
    unsigned int event_num = event->GetEventID();

    EventScores result;
    G4HCofThisEvent* HCE = event->GetHCofThisEvent();
    for (int i = 0; i < HCE->GetNumberOfCollections(); i++)
    {
        auto const* hc = (OpticalHitsCollection*) HCE->GetHC(i);
        result.emplace(hc->GetSDname(), hc->scores());
        G4cout << "recorded event " << event_num << ": " << hc->GetSDname() << " (" << hc->scores().num_cherenkov << ", " << hc->scores().num_scintillation << ")" << G4endl;
    }

    scores_.emplace(event_num, result);
}

void OpticalRun::Merge(G4Run const* run)
{
    OpticalRun* opt_run = (OpticalRun*) run;
    G4cout << "Merging run w/ " << opt_run->scores().size() << " events" << G4endl;
    for (auto const& score : opt_run->scores())
    {
        scores_.insert(score);
    }
}

auto OpticalRun::scores() const -> std::map<unsigned int, EventScores> const&
{
    return scores_;
}

RunAction::RunAction(inp::Config const& config, PrimaryGeneratorAction* primary_action)
    : config_(config)
    , primary_action_(primary_action)
{
    // IsMaster isn't established at constructor. Master thread doesn't have primary action so use
    // that as indicator instead
    if (!primary_action)
    {
        run_writer_ = std::make_unique<RootRunWriter>(config_.output.output_filename, config_.detector.detectors);
    }
}

G4Run* RunAction::GenerateRun()
{
    return new OpticalRun();
}

void RunAction::BeginOfRunAction(G4Run const* run)
{
    if (primary_action_)
    {
        primary_action_->set_energy(config_.primary.energies[run->GetRunID()]);
    }
}

void RunAction::EndOfRunAction(G4Run const* run)
{
    if (this->IsMaster())
    {
        OpticalRun* opt_run = (OpticalRun*) run;

        G4cout << "End of run action. Saving " << opt_run->scores().size() << " events." << G4endl;

        run_writer_->start_run(config_.primary.energies[opt_run->GetRunID()]);

        for (unsigned int i = 0; i < opt_run->scores().size(); i++)
        {
            // TODO: don't need event number? just flatten to a vector?
            run_writer_->add_event_score(opt_run->scores().at(i));
        }

        run_writer_->end_run();
    }
}
