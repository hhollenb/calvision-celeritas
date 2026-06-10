#pragma once

#include <G4ThreeVector.hh>

#include <map>
#include <string>
#include <vector>
#include <unordered_set>

namespace inp
{

struct Primary
{
    int pdg;
    G4ThreeVector direction;
    G4ThreeVector position;

    std::vector<double> energies;
};

struct Detector
{
    std::string geometry_filename;
    std::map<std::string, std::string> detectors;
    std::unordered_set<std::string> allowed_volumes;
};

struct Output
{
    std::string output_filename;
    bool record_geant4{false};
};

struct Config
{
    unsigned int num_events;
    Primary primary;
    Detector detector;
    Output output;
};

} // namespace inp
