#include "LeakageSensitiveDetector.hh"

#include "G4Positron.hh"
#include "G4SystemOfUnits.hh"

ShowerHitsCollection::ShowerHitsCollection(std::string const& det_name, std::string const& collection_name)
    : G4VHitsCollection(det_name, collection_name)
    , total_energy_(0)
    , num_particles_(0)
{}

void ShowerHitsCollection::score(double energy)
{
    total_energy_ += energy;
    if (energy > 100 * MeV)
    {
        num_particles_ += 1;
    }
}

double ShowerHitsCollection::total_energy() const
{
    return total_energy_;
}

unsigned int ShowerHitsCollection::num_particles() const
{
    return num_particles_;
}


ShowerLeakageDetector::ShowerLeakageDetector(std::string const& name)
    : G4VSensitiveDetector(name)
{
    collectionName.insert("Leakage");
}

void ShowerLeakageDetector::Initialize(G4HCofThisEvent* HCE)
{
    hc_ = nullptr;

    if (hc_id_ < 0)
    {
        hc_id_ = this->GetCollectionID(0);
    }

    hc_ = new ShowerHitsCollection(this->GetName(), "Leakage");
    HCE->AddHitsCollection(hc_id_, hc_);
}

void ShowerLeakageDetector::EndOfEvent(G4HCofThisEvent* HCE)
{
}

bool ShowerLeakageDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    double energy = step->GetPreStepPoint()->GetTotalEnergy();

    // positron energy correction
    if (step->GetTrack()->GetParticleDefinition() == G4Positron::Definition())
    {
        energy += 2 * G4Positron::Definition()->GetPDGMass();
    }

    hc_->score(energy);
    step->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
    return true;
}
