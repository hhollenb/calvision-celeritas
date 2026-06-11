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
    std::vector<std::string> allowed_volumes;
};

struct Output
{
    std::string output_filename;
    bool record_geant4{false};
    bool record_celeritas;
};

struct Config
{
    unsigned int num_events;
    unsigned int num_threads{1};
    Primary primary;
    Detector detector;
    Output output;
};

Config from_json_file(std::string const& filename);

} // namespace inp
