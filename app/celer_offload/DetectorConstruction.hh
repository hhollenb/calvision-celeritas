#pragma once

#include <memory>

#include <G4VUserDetectorConstruction.hh>
#include <G4GDMLParser.hh>
#include <G4VPhysicalVolume.hh>

#include "Config.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction(inp::Config const& config);

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

  private:
    G4VPhysicalVolume* phys_vol_world_;
    G4GDMLParser gdml_parser_;

    inp::Config config_;
};
