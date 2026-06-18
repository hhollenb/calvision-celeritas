#include "EdepSensitiveDetector.hh"

#include "G4SystemOfUnits.hh"

EdepHitCollection::EdepHitCollection(std::string const& det_name, std::string const& collection_name)
    : G4VHitsCollection(det_name, collection_name)
{}

void EdepHitCollection::score(G4Step const& step)
{
    auto const* ps = step.GetPreStepPoint();

    EdepHit hit;
    hit.pos_x = ps->GetPosition()[0] / cm;
    hit.pos_y = ps->GetPosition()[1] / cm;
    hit.pos_z = ps->GetPosition()[2] / cm;
    hit.edep = step.GetTotalEnergyDeposit() / GeV;
    hits_.push_back(hit);
}

std::vector<EdepHit> const& EdepHitCollection::hits() const
{
    return hits_;
}


EdepHitDetector::EdepHitDetector(std::string const& name)
    : G4VSensitiveDetector(name)
{
    collectionName.insert("Edep");
}

void EdepHitDetector::Initialize(G4HCofThisEvent* HCE)
{
    hc_ = nullptr;

    if (hc_id_ < 0)
    {
        hc_id_ = this->GetCollectionID(0);
    }

    hc_ = new EdepHitCollection(this->GetName(), "Edep");
    HCE->AddHitsCollection(hc_id_, hc_);
}

void EdepHitDetector::EndOfEvent(G4HCofThisEvent* HCE)
{
}

bool EdepHitDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    if (step->GetTotalEnergyDeposit() > 0)
    {
        hc_->score(*step);
    }
    return true;
}
