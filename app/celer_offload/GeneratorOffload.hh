#pragma once

#include <optional>
#include <vector>
#include <unordered_set>

#include <G4Scintillation.hh>
#include <G4Cerenkov.hh>

#include "celeritas/optical/Types.hh"

namespace inp
{
class Config;
} // namespace inp

class BaseGeneratorOffload
{
  public:
    using AllowedVolNames = std::vector<std::string>;
    using AllowedVols = std::unordered_set<G4LogicalVolume const*>;

    struct Options
    {
        bool track_celer;
        bool track_g4;
        std::optional<AllowedVolNames> allowed_names{};

        static Options from_config(inp::Config);
    };

  public:
    BaseGeneratorOffload(celeritas::GeneratorType gen_type, Options opts);

    void make_allowed_volumes();
    bool is_allowed_volume(G4Step const& step) const;
    bool track_g4() const { return track_g4_; }
    bool track_celer() const { return track_celer_; }

    void offload(G4Track const& track, G4Step const& step, unsigned int num_photons);

  private:
    std::optional<AllowedVolNames> allowed_names_;
    std::optional<AllowedVols> allowed_vols_;
    celeritas::GeneratorType gen_type_;
    bool track_g4_;
    bool track_celer_;
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
