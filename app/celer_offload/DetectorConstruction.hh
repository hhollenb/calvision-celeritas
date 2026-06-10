#pragma once

#include <memory>

#include <G4VUserDetectorConstruction.hh>
#include <G4GDMLParser.hh>
#include <G4VPhysicalVolume.hh>

#include "Config.hh"
#include "OpticalSensitiveDetector.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction(inp::Config const& config, OpticalHitRecorder* hit_recorder);

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

  private:
    G4VPhysicalVolume* phys_vol_world_;
    G4GDMLParser gdml_parser_;

    inp::Config config_;
    OpticalHitRecorder* hit_recorder_;
};
