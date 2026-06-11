#include "DetectorConstruction.hh"

#include <G4SDManager.hh>

#include "OpticalSensitiveDetector.hh"

DetectorConstruction::DetectorConstruction(inp::Config const& config, OpticalHitRecorder* hit_recorder)
    : config_(config)
    , hit_recorder_(hit_recorder)
{
    gdml_parser_.SetStripFlag(true);
    gdml_parser_.Read(config_.detector.geometry_filename, false);
    phys_vol_world_ = gdml_parser_.GetWorldVolume();
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    return phys_vol_world_;
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
            G4VSensitiveDetector* this_sd = new SignalSensitiveDetector(sd_name, hit_recorder_, config_);
            sd_manager->AddNewDetector(this_sd);
            G4VUserDetectorConstruction::SetSensitiveDetector(log_vol->GetName(), this_sd);

            G4cout << "Constructed sensitive detector " << sd_name << " associated with logical volume " << log_vol->GetName() << G4endl;
        }
    }
}
