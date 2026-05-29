#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <iostream>

#include <G4TouchableHistory.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>

#include "geocel/GeantGeoParams.hh"

class DetectorMapping
{
  public:
    static std::unique_ptr<DetectorMapping> full_matrix()
    {
        return std::make_unique<DetectorMapping>(std::map<std::string, std::string>{
                {"detector", "rsc_matrix_tower"},
                {"fsc_detector", "fsc_matrix_tower"},
            });
    }

    DetectorMapping(std::map<std::string, std::string> detector_parent_names);

    // Construct after global_geant_geo has been initialized
    void construct();

    unsigned int operator()(G4TouchableHistory const*);
    unsigned int operator()(celeritas::VolumeUniqueInstanceId);

    unsigned int num_detectors() const { return num_detectors_; }

  private:
    std::map<std::string, std::string> parent_names_;

    unsigned int num_detectors_;
    std::map<G4LogicalVolume const*, G4LogicalVolume const*> volume_parent_map_;
    std::map<G4VPhysicalVolume const*, unsigned int> detector_map_;

    std::vector<celeritas::VolumeInstanceId> scratch_;
    std::shared_ptr<celeritas::GeantGeoParams const> geant_geo_;

    template<class T>
    unsigned int lookup_impl(T const& parser) const;
};


template<class T>
unsigned int DetectorMapping::lookup_impl(T const& parser) const
{
    auto const* parent_vol = volume_parent_map_.at(parser(0)->GetLogicalVolume());
    for (unsigned int i = 0; i < parser.size(); i++)
    {
        auto const* pv = parser(i);
        if (pv->GetLogicalVolume() == parent_vol)
        {
            return detector_map_.at(pv);
        }
    }
    return static_cast<unsigned int>(-1);
}
