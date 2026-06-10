#pragma once

#include <string>
#include <memory>
#include <map>

#include <G4Run.hh>
#include <G4UserRunAction.hh>

#include "Config.hh"
#include "OpticalHit.hh"
#include "PrimaryGeneratorAction.hh"
#include "RootSignalWriter.hh"

class SignalRun : public G4Run
{
  public:
    SignalRun();

    void RecordEvent(G4Event const*) override;
    void Merge(G4Run const*) override;

    std::map<unsigned int, EventHistograms> const& event_hists() const;

  private:
    std::map<unsigned int, EventHistograms> event_hists_;
};

class SignalRunAction : public G4UserRunAction
{
  public:
    SignalRunAction(inp::Config const& config, PrimaryGeneratorAction* primary_action);

    G4Run* GenerateRun() override;

    void BeginOfRunAction(G4Run const*) override;
    void EndOfRunAction(G4Run const*) override;

  private:
    inp::Config config_;
    PrimaryGeneratorAction* primary_action_;

    std::unique_ptr<RootSignalWriter> root_writer_;
};
