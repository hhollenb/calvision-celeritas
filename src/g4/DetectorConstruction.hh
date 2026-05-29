#pragma once

#include <memory>

#include <G4VUserDetectorConstruction.hh>
#include <G4GDMLParser.hh>
#include <G4VPhysicalVolume.hh>

#include "../DetectorMapping.hh"
#include "../OpticalHitRecorder.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction(std::string const& filename, OpticalHitRecorder* record_hit, DetectorMapping* detector_mapping);

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

  private:
    // Assigned by constructor and released ownership at Construct()
    std::unique_ptr<G4VPhysicalVolume> phys_vol_world_;
    G4GDMLParser gdml_parser_;

    OpticalHitRecorder* record_hit_;
    DetectorMapping* detector_mapping_;
};
