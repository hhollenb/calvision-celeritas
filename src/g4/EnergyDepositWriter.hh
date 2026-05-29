#pragma once

#include <memory>
#include <string>
#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "../EnergyDepositData.hh"

class EnergyDepositWriter
{
  public:
    EnergyDepositWriter();
    ~EnergyDepositWriter();

    void operator()(celeritas::optical::GeneratorDistributionData const&);

    void open(std::string const& filename);
    void close();

  private:
    std::unique_ptr<TFile> file_;
    TTree* tree_;

    EdepData buffer_;
};
