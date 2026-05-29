#include "DetectorMapping.hh"

#include <G4LogicalVolumeStore.hh>
#include <G4PhysicalVolumeStore.hh>

#include "geocel/VolumeParams.hh"
#include "geocel/VolumePathFinder.hh"

#include <iostream>

struct GeantHistoryParser
{
    G4TouchableHistory const* history;

    G4VPhysicalVolume const* operator()(unsigned int i) const
    {
        return history->GetVolume(i);
    }

    unsigned int size() const { return history->GetHistoryDepth(); }
};

struct CeleritasHistoryParser
{
    celeritas::GeantGeoParams const& geant_geo_;
    celeritas::Span<celeritas::VolumeInstanceId> path;

    G4VPhysicalVolume const* operator()(unsigned int i) const
    {
        return geant_geo_.id_to_geant(path[path.size() - 1 - i]);
    }

    unsigned int size() const { return path.size(); }
};

DetectorMapping::DetectorMapping(std::map<std::string, std::string> detector_parent_names)
    : parent_names_(std::move(detector_parent_names))
{}

void DetectorMapping::construct()
{
    geant_geo_ = celeritas::global_geant_geo().lock();
    scratch_ = std::vector<celeritas::VolumeInstanceId>(geant_geo_->volumes()->num_volume_levels() - 1);

    auto const* lv_store = G4LogicalVolumeStore::GetInstance();
    auto const* pv_store = G4PhysicalVolumeStore::GetInstance();

    num_detectors_ = 0;

    std::cout << "Constructing detector mapping...\n";
    std::cout << "Num physical volumes: " << pv_store->size() << "\n";
    for (auto const& vals : parent_names_)
    {
        std::cout << "Detector " << vals.first << ", parent " << vals.second << "\n";
        auto* detector_volume = lv_store->GetVolume(vals.first, false);
        auto* parent_volume = lv_store->GetVolume(vals.second, false);

        volume_parent_map_.emplace(detector_volume, parent_volume);

        for (auto const* pv : *pv_store)
        {
            if (pv->GetLogicalVolume() == parent_volume)
            {
                detector_map_.emplace(pv, num_detectors_++);
            }
        }
    }
    std::cout << "Done. Number of detectors: " << num_detectors_ << "\n";
}

unsigned int DetectorMapping::operator()(G4TouchableHistory const* history)
{
    return this->lookup_impl(GeantHistoryParser{history});
}

unsigned int DetectorMapping::operator()(celeritas::VolumeUniqueInstanceId vid)
{
    celeritas::VolumePathFinder find_path{geant_geo_->volumes()->host_ref(), celeritas::make_span(scratch_)};
    return this->lookup_impl(CeleritasHistoryParser{*geant_geo_, find_path(vid)});
}
