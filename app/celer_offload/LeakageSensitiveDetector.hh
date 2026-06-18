#pragma once

#include <G4VSensitiveDetector.hh>
#include <G4VHitsCollection.hh>


class ShowerHitsCollection : public G4VHitsCollection
{
  public:
    ShowerHitsCollection(std::string const& det_name, std::string const& collection_name);

    void score(double energy);

    double total_energy() const;
    unsigned int num_particles() const;

  private:
    double total_energy_;
    unsigned int num_particles_;
};

class ShowerLeakageDetector : public G4VSensitiveDetector
{
  public:
    ShowerLeakageDetector(std::string const& name);

    bool ProcessHits(G4Step* step, G4TouchableHistory*) override;

    void Initialize(G4HCofThisEvent* HCE) override;
    void EndOfEvent(G4HCofThisEvent* HCE) override;

  private:
    int hc_id_{-1};
    ShowerHitsCollection* hc_;
};
