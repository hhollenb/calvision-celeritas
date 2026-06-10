#pragma once

#include <string>
#include <map>
#include <memory>

#include "TFile.h"
#include "TTree.h"
#include "TVector.h"

#include "OpticalHit.hh"

class RootRunWriter
{
  public:
    using EventScores = std::map<std::string, DetectorScoring>;

    RootRunWriter(std::string const& filename, std::map<std::string, std::string> const& detectors);
    ~RootRunWriter();

    void start_run(double energy);
    void add_event_score(EventScores const& scores);
    void end_run();

    void close();

  private:
    std::unique_ptr<TFile> file_;
    std::vector<double> energies_;
    unsigned int run_num_;
    TTree* run_tree_;
    std::map<std::string, DetectorScoring> buffer_;
    std::map<std::string, std::string> detectors_;
};
