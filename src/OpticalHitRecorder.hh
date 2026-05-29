#pragma once

#include <vector>
#include <memory>

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"

#include "celeritas/optical/Types.hh"

struct OpticalHit
{
    double energy;
    double time;
    unsigned int detector_id;
    celeritas::GeneratorType gen_type{celeritas::GeneratorType::size_};
};

class DetectorHistograms
{
  public:
    DetectorHistograms(std::string const& name, TFile* file);

    void operator()(OpticalHit const& hit);

  private:
    TH1D* hist_energy_;
    TH1D* hist_time_;
    TH1D* hist_signal_;
};

class TotalOpticalHistograms
{
  public:
    TotalOpticalHistograms(TFile* file);

    void operator()(OpticalHit const& hit);

  private:
    TH2D* rsc_heat_map_;
    TH2D* fsc_heat_map_;

    DetectorHistograms hist_total_;

    DetectorHistograms hist_scint_;
    DetectorHistograms hist_cheren_;

    DetectorHistograms hist_rear_;
    DetectorHistograms hist_front_;
};

class OpticalHitRecorder
{
  public:
    OpticalHitRecorder(std::string const& name);

    void set_num_detectors(unsigned int n) { num_detectors_ = n; }

    void start_event(unsigned int event_id);
    void end_event();

    void operator()(OpticalHit const& hit);

  private:
    std::string name_;
    std::unique_ptr<TFile> file_;

    unsigned int num_detectors_;
    // std::vector<DetectorHistograms> hists_;
    std::unique_ptr<TotalOpticalHistograms> total_hists_;
};
