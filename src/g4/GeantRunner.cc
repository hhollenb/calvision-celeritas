#include "GeantRunner.hh"

#include <G4TransportationManager.hh>

#include "geocel/GeantGeoParams.hh"

#include "EnergyDepositWriter.hh"
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

GeantRunner::GeantRunner()
    : edep_writer_(std::make_unique<EnergyDepositWriter>())

{
    detector_mapping_ = DetectorMapping::full_matrix();
    hit_recorder_ = std::make_unique<OpticalHitRecorder>("geant");

    run_manager_ = std::unique_ptr<G4RunManager>{G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial)};

    run_manager_->SetUserInitialization(new DetectorConstruction("dual_readout.gdml", hit_recorder_.get(), detector_mapping_.get()));
    run_manager_->SetUserInitialization(new PhysicsList(edep_writer_.get()));
    run_manager_->SetUserInitialization(new ActionInitialization(edep_writer_.get(),
                                                                 hit_recorder_.get()));

    run_manager_->Initialize();

    auto tmgr = G4TransportationManager::GetTransportationManager();
    std::cout << "Number of world: " << tmgr->GetNoWorlds() << "\n";

    geo_params_ = std::make_shared<celeritas::GeantGeoParams>(*tmgr->GetWorldsIterator(), celeritas::Ownership::reference);
    celeritas::global_geant_geo(geo_params_);
    if (!celeritas::global_geant_geo().lock())
    {
        std::cout << "Failed to set global geant geo...\n";
    }

    detector_mapping_->construct();
    hit_recorder_->set_num_detectors(detector_mapping_->num_detectors());
}

void GeantRunner::operator()()
{
    run_manager_->BeamOn(1);
}
