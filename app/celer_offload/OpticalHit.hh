#pragma once

#include <vector>
#include <string>
#include <map>

#include <G4VHitsCollection.hh>

#include "celeritas/optical/Types.hh"

#include "TProfile.h"
#include "TH1.h"


struct OpticalHit
{
    celeritas::GeneratorType gen_type;
    double energy;
    double time;
};

struct ProcessHistograms
{
    TH1D* energy;
    TH1D* time;
    TProfile* signal;

    ProcessHistograms(celeritas::GeneratorType gen_type, std::string const& det_name);
    void fill(OpticalHit const& hit);
    void write(TDirectory* dir) const;
};

struct SignalHistograms
{
    ProcessHistograms cherenkov;
    ProcessHistograms scintillation;

    SignalHistograms(std::string const& det_name);
    void fill(OpticalHit const& hit);
};

class SignalHitsCollection : public G4VHitsCollection
{
  public:
    SignalHitsCollection(std::string const& det_name, std::string const& collection_name);

    void score(OpticalHit const& hit);

    SignalHistograms const& hists() const;

    std::string const& id_name() const;

  private:
    SignalHistograms hists_;
    std::string id_name_;
};

using EventHistograms = std::map<std::string, SignalHistograms>;
