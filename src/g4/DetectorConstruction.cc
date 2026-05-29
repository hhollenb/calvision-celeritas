#include "DetectorConstruction.hh"

#include <G4SDManager.hh>

#include "OpticalSensitiveDetector.hh"

DetectorConstruction::DetectorConstruction(std::string const& filename, OpticalHitRecorder* record_hit, DetectorMapping* detector_mapping)
    : record_hit_(record_hit)
    , detector_mapping_(detector_mapping)
{
    gdml_parser_.SetStripFlag(true);
    gdml_parser_.Read(filename, false);
    phys_vol_world_.reset(gdml_parser_.GetWorldVolume());
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    return phys_vol_world_.release();
}

void DetectorConstruction::ConstructSDandField()
{
    auto sd_manager = G4SDManager::GetSDMpointer();
    auto const aux_map = gdml_parser_.GetAuxMap();

    for (auto iter = aux_map->begin(); iter != aux_map->end(); iter++)
    {
        auto const& log_vol = iter->first;

        for (auto const& element : iter->second)
        {
            if (element.type != "SensDet")
            {
                continue;
            }

            // Add sensitive detector
            std::string sd_name = log_vol->GetName();
            auto this_sd = new OpticalSensitiveDetector(sd_name, record_hit_, detector_mapping_);
            sd_manager->AddNewDetector(this_sd);
            G4VUserDetectorConstruction::SetSensitiveDetector(log_vol->GetName(), this_sd);

            std::cout << "Constructed sensitive detector " << sd_name << " associated with logical volume " << log_vol->GetName() << "\n";
        }
    }
}
