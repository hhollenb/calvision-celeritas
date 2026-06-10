#include "Runner.hh"

#include <G4MTRunManager.hh>
#include <G4RunManagerFactory.hh>

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

Runner::Runner(inp::Config config)
    : config_(std::move(config))
{
    run_manager_.reset(G4RunManagerFactory::CreateRunManager(G4RunManagerType::MT));

    if (auto* rm_mt = (G4MTRunManager*) run_manager_.get())
    {
        rm_mt->SetNumberOfThreads(4);
    }

    run_manager_->SetUserInitialization(new DetectorConstruction(config_));
    run_manager_->SetUserInitialization(new PhysicsList(config_.detector.allowed_volumes));
    run_manager_->SetUserInitialization(new ActionInitialization(config_));

    run_manager_->Initialize();
}

void Runner::operator()()
{
    for (unsigned int i = 0; i < config_.primary.energies.size(); i++)
    {
        G4cout << "Run " << i << " energy " << config_.primary.energies[i] << G4endl;
        run_manager_->BeamOn(config_.num_events);
    }
}
