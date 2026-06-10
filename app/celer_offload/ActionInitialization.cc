#include "ActionInitialization.hh"

// #include "TROOT.h"
#include "TH1.h"

#include "EventAction.hh"
#include "SignalRunAction.hh"
#include "PrimaryGeneratorAction.hh"

ActionInitialization::ActionInitialization(inp::Config const& config)
    : config_(config)
{}

void ActionInitialization::BuildForMaster() const
{
    // ROOT::DisableObjectAutoRegistration();
    TH1::AddDirectory(false);

    G4cout << "Building for master..." << G4endl;
    this->SetUserAction(new SignalRunAction(config_, nullptr));
}

void ActionInitialization::Build() const
{
    // ROOT::DisableObjectAutoRegistration();
    TH1::AddDirectory(false);

    G4cout << "Building for worker..." << G4endl;
    PrimaryGeneratorAction* primary_action = new PrimaryGeneratorAction(config_.primary);
    this->SetUserAction(new SignalRunAction(config_, primary_action));
    this->SetUserAction(primary_action);
    this->SetUserAction(new EventAction());
}
