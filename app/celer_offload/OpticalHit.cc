#include "OpticalHit.hh"

#include <iostream>

std::string to_name(celeritas::GeneratorType gen_type)
{
    switch (gen_type)
    {
        case celeritas::GeneratorType::cherenkov: return "cherenkov";
        case celeritas::GeneratorType::scintillation: return "scintillation";
        default: return "none";
    }
}

std::string to_title(celeritas::GeneratorType gen_type)
{
    switch (gen_type)
    {
        case celeritas::GeneratorType::cherenkov: return "Cherenkov";
        case celeritas::GeneratorType::scintillation: return "Scintillation";
        default: return "None";
    }
}


ProcessHistograms::ProcessHistograms(celeritas::GeneratorType gen_type, std::string const& det_name)
{
    energy = new TH1D((det_name + "_" + to_name(gen_type) + "_energy").c_str(),
                      (det_name + " " + to_title(gen_type) + " Energy;Energy [eV];Num Photons").c_str(),
                      100, 1.0, 6.0);
    energy->SetDirectory(nullptr);

    time = new TH1D((det_name + "_" + to_name(gen_type) + "_time").c_str(),
                    (det_name + " " + to_title(gen_type) + " Time;Time [ns];Num Photons").c_str(),
                    100, 0.0, 100.0);
    time->SetDirectory(nullptr);

    signal = new TProfile((det_name + "_" + to_name(gen_type) + "_signal").c_str(),
                          (det_name + " " + to_title(gen_type) + " Signal;Time [ns];Energy [eV]").c_str(),
                          100, -10.0, 1000.0);
    signal->SetDirectory(nullptr);
}

void ProcessHistograms::fill(OpticalHit const& hit)
{
    energy->Fill(hit.energy);
    time->Fill(hit.time);
    signal->Fill(hit.time, hit.energy);
}

void ProcessHistograms::write(TDirectory* dir) const
{
    energy->SetDirectory(dir);
    energy->Write();

    time->SetDirectory(dir);
    time->Write();

    signal->SetDirectory(dir);
    signal->Write();
}


SignalHistograms::SignalHistograms(std::string const& det_name)
    : cherenkov(celeritas::GeneratorType::cherenkov, det_name)
    , scintillation(celeritas::GeneratorType::scintillation, det_name)
{}

void SignalHistograms::fill(OpticalHit const& hit)
{
    switch (hit.gen_type)
    {
        case celeritas::GeneratorType::cherenkov:
            cherenkov.fill(hit);
            break;
        case celeritas::GeneratorType::scintillation:
            scintillation.fill(hit);
            break;
        default:
            std::cout << "hit without generator type...\n";
            break;
    }
}

SignalHitsCollection::SignalHitsCollection(std::string const& det_name, std::string const& collection_name)
    : G4VHitsCollection(det_name, collection_name)
    , hists_(det_name)
{
}

void SignalHitsCollection::score(OpticalHit const& hit)
{
    hists_.fill(hit);
}

SignalHistograms const& SignalHitsCollection::hists() const 
{
    return hists_;
}
