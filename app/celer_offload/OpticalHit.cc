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

double convert_ev_to_nm(double energy)
{
    return 2 * M_PI * 197.3269804 / energy;
}


ProcessHistograms::ProcessHistograms(celeritas::GeneratorType gen_type, std::string const& det_name, std::string const& collection_name)
{
    energy = new TH1D((det_name + "_" + collection_name + "_" + to_name(gen_type) + "_energy").c_str(),
                      (det_name + " " + collection_name + " " + to_title(gen_type) + " Wavelength;Wavelength [nm];Num Photons").c_str(),
                      100, 200, 1100);
    energy->SetDirectory(nullptr);

    double time_upper_bound = gen_type == celeritas::GeneratorType::cherenkov ? 20.0 : 2500.0;

    time = new TH1D((det_name + "_" + collection_name + "_" + to_name(gen_type) + "_time").c_str(),
                    (det_name + " " + collection_name + " " + to_title(gen_type) + " Time;Time [ns];Num Photons").c_str(),
                    100, 0.0, time_upper_bound);
    time->SetDirectory(nullptr);

    signal = new TH1D((det_name + "_" + collection_name + "_" + to_name(gen_type) + "_signal").c_str(),
                      (det_name + " " + collection_name + " " + to_title(gen_type) + " Signal;Time [ns];Energy [eV]").c_str(),
                      100, 0.0, 2500);
    signal->SetDirectory(nullptr);

    prompt_signal = new TH1D((det_name + "_" + collection_name + "_" + to_name(gen_type) + "_prompt_signal").c_str(),
                             (det_name + " " + collection_name + " " + to_title(gen_type) + " Prompt Signal;Time [ns];Energy [eV]").c_str(),
                             100, 0, 10);
    prompt_signal->SetDirectory(nullptr);
}

void ProcessHistograms::fill(OpticalHit const& hit)
{
    energy->Fill(convert_ev_to_nm(hit.energy));
    time->Fill(hit.time);
    signal->Fill(hit.time, hit.energy);
    prompt_signal->Fill(hit.time, hit.energy);
}

void ProcessHistograms::write(TDirectory* dir) const
{
    energy->SetDirectory(dir);
    energy->Write();

    time->SetDirectory(dir);
    time->Write();

    signal->SetDirectory(dir);
    signal->Write();

    prompt_signal->SetDirectory(dir);
    prompt_signal->Write();
}


SignalHistograms::SignalHistograms(std::string const& det_name, std::string const& collection_name)
    : cherenkov(celeritas::GeneratorType::cherenkov, det_name, collection_name)
    , scintillation(celeritas::GeneratorType::scintillation, det_name, collection_name)
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
    , hists_(det_name, collection_name)
    , id_name_(det_name + "_" + collection_name)
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

std::string const& SignalHitsCollection::id_name() const
{
    return id_name_;
}
