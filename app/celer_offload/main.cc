#include "Runner.hh"

#include "TROOT.h"

int main(void)
{
    ROOT::EnableThreadSafety();


    inp::Config config;
    // config.num_events = 6;

    // config.primary.pdg = 11;
    // config.primary.direction = G4ThreeVector{0, 0, 1};
    // config.primary.position = G4ThreeVector{0, 0, -50};
    // config.primary.energies = {1,};

    // config.detector.geometry_filename = "dual_readout.gdml";
    // config.detector.detectors = {
    //     {"detector", "rear"},
    //     {"fsc_detector", "front"},
    // };
    // config.detector.allowed_volumes = {
    //     "crystal",
    //     "resin_window",
    //     "silicone_gap",
    //     "air_gap",
    //     "fsc_crystal",
    //     "fsc_silicone_gap",
    //     "fsc_resin_window",
    //     "fsc_air_gap",
    // };

    // config.output.output_filename = "output_electrons_geant.root";
    // config.output.record_geant4 = true;

    config = inp::from_json_file("config.json");


    Runner runner{std::move(config)};
    runner();

    return 0;
}
