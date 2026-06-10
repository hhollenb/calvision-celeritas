#pragma once

#include <string>
#include <map>
#include <memory>

#include <G4Run.hh>
#include <G4UserRunAction.hh>

#include "Config.hh"
#include "OpticalHit.hh"
#include "RootRunWriter.hh"
#include "PrimaryGeneratorAction.hh"

class OpticalRun : public G4Run
{
  public:
    using EventScores = std::map<std::string, DetectorScoring>;

  public:
    OpticalRun();

    void RecordEvent(G4Event const*) override;
    void Merge(G4Run const*) override;

    std::map<unsigned int, EventScores> const& scores() const;

  private:
    std::map<unsigned int, EventScores> scores_;
};

class RunAction : public G4UserRunAction
{
  public:
    RunAction(inp::Config const& config, PrimaryGeneratorAction* primary_action);

    G4Run* GenerateRun() override;

    void BeginOfRunAction(G4Run const*) override;
    void EndOfRunAction(G4Run const*) override;

  private:
    std::unique_ptr<RootRunWriter> run_writer_;
    inp::Config config_;
    PrimaryGeneratorAction* primary_action_;
};
