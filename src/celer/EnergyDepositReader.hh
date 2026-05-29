#pragma once

#include <memory>
#include <string>
#include <vector>

#include "celeritas/optical/gen/GeneratorData.hh"

#include "TFile.h"
#include "TTree.h"
#include "Math/Vector3D.h"


class EnergyDepositReader
{
  public:
    EnergyDepositReader() = default;

    std::vector<celeritas::optical::GeneratorDistributionData> operator()(std::string const& filename) const;
};
