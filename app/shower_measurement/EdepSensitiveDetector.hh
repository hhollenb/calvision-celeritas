#pragma once

#include <string>
#include <vector>

#include <G4VSensitiveDetector.hh>
#include <G4VHitsCollection.hh>


struct EdepHit
{
    double pos_x;
    double pos_y;
    double pos_z;
    double edep;
};

class EdepHitCollection : public G4VHitsCollection
{
  public:
    EdepHitCollection(std::string const& det_name, std::string const& collection_name);

    void score(G4Step const& step);

    std::vector<EdepHit> const& hits() const;

  private:
    std::vector<EdepHit> hits_;
};

class EdepHitDetector : public G4VSensitiveDetector
{
  public:
    EdepHitDetector(std::string const& name);

    bool ProcessHits(G4Step* step, G4TouchableHistory*) override;

    void Initialize(G4HCofThisEvent* HCE) override;
    void EndOfEvent(G4HCofThisEvent* HCE) override;

  private:
    int hc_id_{-1};
    EdepHitCollection* hc_;
};
