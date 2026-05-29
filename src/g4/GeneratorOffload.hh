#pragma once

#include <optional>
#include <vector>
#include <unordered_set>

#include <G4Scintillation.hh>
#include <G4Cerenkov.hh>

#include "EnergyDepositWriter.hh"


class BaseGeneratorOffload
{
  public:
    using AllowedVolNames = std::vector<std::string>;
    using AllowedVols = std::unordered_set<G4LogicalVolume const*>;

    struct Options
    {
        EnergyDepositWriter* edep_writer{nullptr};
        std::optional<AllowedVolNames> allowed_names{};
    };

  public:
    BaseGeneratorOffload(celeritas::GeneratorType gen_type, Options opts);

    void make_allowed_volumes();
    bool is_allowed_volume(G4Step const& step) const;

    void offload(G4Track const& track, G4Step const& step, unsigned int num_photons);

  private:
    std::optional<AllowedVolNames> allowed_names_;
    std::optional<AllowedVols> allowed_vols_;
    celeritas::GeneratorType gen_type_;

    EnergyDepositWriter* edep_writer_;
};


class CherenkovOffload : public G4Cerenkov, public BaseGeneratorOffload
{
  public:
    CherenkovOffload(BaseGeneratorOffload::Options opts);

    void PreparePhysicsTable(G4ParticleDefinition const& particle) override;
    G4VParticleChange* PostStepDoIt(G4Track const& track, G4Step const& step) override;
};

class ScintillationOffload : public G4Scintillation, public BaseGeneratorOffload
{
  public:
    ScintillationOffload(BaseGeneratorOffload::Options opts);

    void PreparePhysicsTable(G4ParticleDefinition const& particle) override;
    G4VParticleChange* PostStepDoIt(G4Track const& track, G4Step const& step) override;
    G4VParticleChange* AtRestDoIt(G4Track const& track, G4Step const& step) override;
};
