#include "OpticalHitRecorder.hh"

#include <iostream>

DetectorHistograms::DetectorHistograms(std::string const& name, TFile* file)
{
    hist_energy_ = new TH1D((name + "_energy").c_str(),
                            "Energy;Energy [eV];Num Photons",
                            100,
                            1.0,
                            6.0);
    hist_time_ = new TH1D((name + "_time").c_str(),
                          "Time;Time [ns];Num Photons",
                          100,
                          0.0,
                          100.0);
    hist_signal_ = new TH1D((name + "_signal").c_str(),
                                "Signal;Time [ns];Energy [eV]",
                                100, -10.0, 1000.0);

    hist_energy_->SetDirectory(file);
    hist_time_->SetDirectory(file);
    hist_signal_->SetDirectory(file);
}

void DetectorHistograms::operator()(OpticalHit const& hit)
{
    hist_energy_->Fill(hit.energy);
    hist_time_->Fill(hit.time);
    hist_signal_->Fill(hit.time, hit.energy);
}

TotalOpticalHistograms::TotalOpticalHistograms(TFile* file)
    : hist_total_("total", file)
    , hist_scint_("scintillation", file)
    , hist_cheren_("cherenkov", file)
    , hist_rear_("rear", file)
    , hist_front_("front", file)
{
    rsc_heat_map_ = new TH2D("rsc_heat_map", "Rear Matrix Hit Map;x;y",
                             11, -5, 5,
                             11, -5, 5);
    rsc_heat_map_->SetDirectory(file);

    fsc_heat_map_ = new TH2D("fsc_heat_map", "Front Matrix Hit Map;x;y",
                             5, -2, 2,
                             5, -2, 2);
    fsc_heat_map_->SetDirectory(file);
}

void TotalOpticalHistograms::operator()(OpticalHit const& hit)
{
    if (hit.detector_id < 121)
    {
        int x = (hit.detector_id / 11) - 5;
        int y = (hit.detector_id % 11) - 5;
        rsc_heat_map_->Fill(x, y);

        hist_rear_(hit);
    }
    else
    {
        int d = hit.detector_id - 121;
        int x = (d / 5) - 2;
        int y = (d / 5) - 2;
        fsc_heat_map_->Fill(x, y);

        hist_front_(hit);
    }

    hist_total_(hit);

    if (hit.gen_type == celeritas::GeneratorType::scintillation)
    {
        hist_scint_(hit);
    }
    else if (hit.gen_type == celeritas::GeneratorType::cherenkov)
    {
        hist_cheren_(hit);
    }
}

OpticalHitRecorder::OpticalHitRecorder(std::string const& name)
    : name_(name)
{}

void OpticalHitRecorder::start_event(unsigned int event_id)
{
    std::string filename = name_ + "_output_" + std::to_string(event_id) + ".root";
    file_ = std::make_unique<TFile>(filename.c_str(), "RECREATE", "Optical photon hit histograms");
    // hists_.clear();
    // hists_.reserve(num_detectors_);
    // for (unsigned int i = 0; i < num_detectors_; i++)
    // {
    //     hists_.push_back(DetectorHistograms("det_" + std::to_string(i), file_.get()));
    // }
    total_hists_ = std::make_unique<TotalOpticalHistograms>(file_.get());
}

void OpticalHitRecorder::end_event()
{
    file_->Write();
    file_->Close();
    file_.reset();
}

void OpticalHitRecorder::operator()(OpticalHit const& hit)
{
    // hists_[hit.detector_id](hit);
    (*total_hists_)(hit);
}
