#include "Config.hh"

#include <fstream>
#include <nlohmann/json.hpp>
#include <G4SystemOfUnits.hh>

namespace inp
{

Primary from_json_primary(nlohmann::json const& data)
{
    Primary primary;
    primary.pdg = data.at("pdg").get<int>();
    for (unsigned int i = 0; i < 3; i++)
    {
        primary.direction[i] = data.at("direction")[i].get<double>();
        primary.position[i] = data.at("position")[i].get<double>() * cm;
    }

    for (auto const& energy : data.at("energy"))
    {
        primary.energies.push_back(energy.get<double>() * GeV);
    }

    return primary;
}

Detector from_json_detector(nlohmann::json const& data)
{
    Detector detector;
    detector.geometry_filename = data.at("geometry_filename").get<std::string>();
    
    for (auto const& [key, value] : data.at("detectors").items())
    {
        detector.detectors.emplace(key, value);
    }

    for (auto const& vols : data.at("allowed_volumes"))
    {
        detector.allowed_volumes.push_back(vols.get<std::string>());
    }

    return detector;
}

Output from_json_output(nlohmann::json const& data)
{
    Output output;
    output.output_filename = data.at("output_filename").get<std::string>();
    output.record_geant4 = data.at("record_geant4").get<bool>();
    output.record_celeritas = data.at("record_celeritas").get<bool>();
    return output;
}

Config from_json_file(std::string const& filename)
{
    std::ifstream input_file(filename);
    nlohmann::json data = nlohmann::json::parse(input_file);

    Config result;

    result.num_events = data.at("num_events").get<unsigned int>();
    result.num_threads = data.at("num_threads").get<unsigned int>();

    result.primary = from_json_primary(data.at("primary"));
    result.detector = from_json_detector(data.at("detector"));
    result.output = from_json_output(data.at("output"));

    return result;
}

} // namespace inp
