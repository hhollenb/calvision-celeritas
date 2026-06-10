#pragma once

#include <string>
#include <memory>

#include "TFile.h"
#include "TDirectoryFile.h"
#include "TParameter.h"

#include "OpticalHit.hh"

class RunSignalWriter
{
  public:
    RunSignalWriter(TFile* file, unsigned int run_num, double primary_energy);
    ~RunSignalWriter();

    void operator()(unsigned int event_id, EventHistograms const& hists);

    void close();

  private:
    std::unique_ptr<TDirectoryFile> file_;
    TParameter<double>* energy_;
};


class RootSignalWriter
{
  public:
    RootSignalWriter(std::string const& filename);
    ~RootSignalWriter();

    RunSignalWriter next_run(double primary_energy);

    void close();

  private:
    unsigned int run_num_;
    std::unique_ptr<TFile> file_;
};
