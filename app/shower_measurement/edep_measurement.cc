#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "EdepSensitiveDetector.hh"

#include <G4Positron.hh>
#include <G4Run.hh>
#include <G4UserRunAction.hh>
#include <G4RunManager.hh>
#include <G4MTRunManager.hh>
#include <G4RunManagerFactory.hh>
#include <G4SystemOfUnits.hh>
#include <FTFP_BERT.hh>
#include <G4VUserPrimaryGeneratorAction.hh>
#include <G4ParticleGun.hh>
#include <G4VUserActionInitialization.hh>
#include <G4VUserDetectorConstruction.hh>
#include <G4GDMLParser.hh>
#include <G4VPhysicalVolume.hh>
#include <G4SDManager.hh>
#include <G4LogicalVolumeStore.hh>


struct Config
{
    int num_events;
    int num_threads;
    int pdg_id;
    double energy;
    std::string geometry_filename;
    std::string output_filename;

    static Config from_inp_file(std::string const& filename)
    {
        std::ifstream input_file(filename);
        nlohmann::json data = nlohmann::json::parse(input_file);

        Config config;
        config.num_events = data.at("num_events").get<int>();
        config.num_threads = data.at("num_threads").get<int>();
        config.pdg_id = data.at("pdg_id").get<int>();
        config.energy = data.at("energy").get<double>() * GeV;
        config.geometry_filename = data.at("geometry_filename").get<std::string>();
        config.output_filename = data.at("output_filename").get<std::string>();

        return config;
    }
};

class LeakageHitCollection : public G4VHitsCollection
{
  public:
    LeakageHitCollection(std::string const& det_name, std::string const& collection_name)
        : G4VHitsCollection(det_name, collection_name)
    {
    }

    void score(G4Step const& step)
    {
        auto const* ps = step.GetPreStepPoint();

        EdepHit hit;
        hit.pos_x = ps->GetPosition()[0] / cm;
        hit.pos_y = ps->GetPosition()[1] / cm;
        hit.pos_z = ps->GetPosition()[2] / cm;
        // hit.edep = ps->GetTotalEnergy() / GeV;
        hit.edep = ps->GetKineticEnergy() / GeV;

        if (step.GetTrack()->GetParticleDefinition() == G4Positron::Definition())
        {
            hit.edep += 2 * G4Positron::Definition()->GetPDGMass() / GeV;
        }

        hits_.push_back(hit);
    }

    std::vector<EdepHit> const& hits() const
    {
        return hits_;
    }

  private:
    std::vector<EdepHit> hits_;
};


class ShowerLeakageDetector : public G4VSensitiveDetector
{
  public:
    ShowerLeakageDetector(std::string const& name)
        : G4VSensitiveDetector(name) 
    {
        collectionName.insert("Leakage");
    }

    bool ProcessHits(G4Step* step, G4TouchableHistory*) override
    {
        hc_->score(*step);
        step->GetTrack()->SetTrackStatus(fKillTrackAndSecondaries);
        return true;
    }

    void Initialize(G4HCofThisEvent* HCE) override
    {
        hc_ = nullptr;

        if (hc_id_ < 0)
        {
            hc_id_ = this->GetCollectionID(0);
        }

        hc_ = new LeakageHitCollection(this->GetName(), "Leakage");
        HCE->AddHitsCollection(hc_id_, hc_);
    }

    void EndOfEvent(G4HCofThisEvent* HCE) override {}

  private:
    int hc_id_{-1};
    LeakageHitCollection* hc_;
};


class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction(Config const& config)
    {
        gdml_parser_.SetStripFlag(true);
        gdml_parser_.Read(config.geometry_filename, false);
        phys_vol_world_ = gdml_parser_.GetWorldVolume();
    }

    G4VPhysicalVolume* Construct() override
    {
        return phys_vol_world_;
    }

    void ConstructSDandField() override
    {
        auto sd_manager = G4SDManager::GetSDMpointer();
        auto lv_store = G4LogicalVolumeStore::GetInstance();

        std::vector<std::string> lv_names{"fsc_crystal", "rsc_crystal"};

        for (auto const& name : lv_names)
        {
            auto* lv = lv_store->GetVolume(name, false);
            auto* sd = new EdepHitDetector(lv->GetName());
            sd_manager->AddNewDetector(sd);
            this->SetSensitiveDetector(lv->GetName(), sd);
        }

        auto const aux_map = gdml_parser_.GetAuxMap();
        for (auto iter = aux_map->begin(); iter != aux_map->end(); iter++)
        {
            auto const& log_vol = iter->first;
            for (auto const& element : iter->second)
            {
                if (element.type != "SensDet" || element.value != "ShowerLeakageDetector")
                {
                    continue;
                }

                std::string sd_name = log_vol->GetName();
                G4VSensitiveDetector* this_sd = new ShowerLeakageDetector(sd_name);

                // Add sensitive detector
                sd_manager->AddNewDetector(this_sd);
                G4VUserDetectorConstruction::SetSensitiveDetector(log_vol->GetName(), this_sd);

                G4cout << "Constructed " << element.value << " sensitive detector " << sd_name << " associated with logical volume " << log_vol->GetName() << G4endl;
            }
        }
    }

  private:
    G4VPhysicalVolume* phys_vol_world_;
    G4GDMLParser gdml_parser_;
};

struct EventData
{
    std::vector<EdepHit> hits;
    std::vector<EdepHit> lateral_leakage;
    std::vector<EdepHit> longitudinal_leakage;
};

class EdepRun : public G4Run
{
  public:
    EdepRun()
        : G4Run()
    {}

    void RecordEvent(G4Event const* event) override
    {
        unsigned int event_num = event->GetEventID();
        EventData result;
        G4HCofThisEvent* HCE = event->GetHCofThisEvent();
        for (int i = 0; i < HCE->GetNumberOfCollections(); i++)
        {
            if (auto* hc = dynamic_cast<EdepHitCollection const*>(HCE->GetHC(i)))
            {
                for (auto const& hit : hc->hits())
                {
                    result.hits.push_back(hit);
                }
            }
            else if (auto* hc = dynamic_cast<LeakageHitCollection const*>(HCE->GetHC(i)))
            {
                if (hc->GetSDname() == "lateral_leakage")
                {
                    for (auto const& leak : hc->hits())
                    {
                        result.lateral_leakage.push_back(leak);
                    }
                }
                else if (hc->GetSDname() == "longitudinal_leakage")
                {
                    for (auto const& leak : hc->hits())
                    {
                        result.longitudinal_leakage.push_back(leak);
                    }
                }
            }
        }
        hits_.emplace(event_num, std::move(result));
    }

    void Merge(G4Run const* run) override
    {
        EdepRun const* edep_run = dynamic_cast<EdepRun const*>(run);
        for (auto const& hit : edep_run->hits())
        {
            hits_.insert(hit);
        }
    }

    std::map<unsigned int, EventData> const& hits() const
    {
        return hits_;
    }

  private:
    std::map<unsigned int, EventData> hits_;
};

class TreeWriter
{
  public:
    TreeWriter(std::string const& name, TFile* file)
    {
        tree_ = new TTree(name.c_str(), "Edep data");
        tree_->SetDirectory(file);
        tree_->Branch<double>("pos_x", &hit_.pos_x);
        tree_->Branch<double>("pos_y", &hit_.pos_y);
        tree_->Branch<double>("pos_z", &hit_.pos_z);
        tree_->Branch<double>("edep", &hit_.edep);
    }

    ~TreeWriter()
    {
        tree_->Write();
    }

    void operator()(EdepHit const& hit)
    {
        hit_ = hit;
        tree_->Fill();
    }

  private:
    EdepHit hit_;
    TTree* tree_;
};

class RunAction : public G4UserRunAction
{
  public:
    RunAction(Config const& config)
        : config_(config)
    {}

    G4Run* GenerateRun() override
    {
        return new EdepRun();
    }

    void BeginOfRunAction(G4Run const* run) override
    {
    }

    void EndOfRunAction(G4Run const* run) override
    {
        if (this->IsMaster())
        {
            std::unique_ptr<TFile> file_ = std::make_unique<TFile>(config_.output_filename.c_str(), "RECREATE", "Edeps");
            EdepRun const* edep_run = dynamic_cast<EdepRun const*>(run);

            for (auto const& [event_num, data] : edep_run->hits())
            {
                {
                    TreeWriter write_hit{"event_" + std::to_string(event_num), file_.get()};
                    for (auto const& hit : data.hits)
                    {
                        write_hit(hit);
                    }
                }

                {
                    TreeWriter write_leak{"event_lateral_leak_" + std::to_string(event_num), file_.get()};
                    for (auto const& leak : data.lateral_leakage)
                    {
                        write_leak(leak);
                    }
                }

                {
                    TreeWriter write_leak{"event_longitudinal_leak_" + std::to_string(event_num), file_.get()};
                    for (auto const& leak : data.longitudinal_leakage)
                    {
                        write_leak(leak);
                    }
                }
            }

            file_->Close();
        }
    }

  private:
    Config config_;
};

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(Config const& config)
        : G4VUserPrimaryGeneratorAction()
    {
        particle_gun_ = std::make_shared<G4ParticleGun>(1);

        particle_gun_->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(config.pdg_id));
        particle_gun_->SetParticlePosition(G4ThreeVector{0, 0, -50} * cm);
        particle_gun_->SetParticleMomentumDirection(G4ThreeVector{0, 0, 1});
        particle_gun_->SetParticleEnergy(config.energy);
    }

    void GeneratePrimaries(G4Event* event)
    {
        particle_gun_->GeneratePrimaryVertex(event);
    }

  private:
    std::shared_ptr<G4ParticleGun> particle_gun_;
};

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(Config const& config)
            : config_(config)
    {
    }

    void BuildForMaster() const override
    {
        this->SetUserAction(new RunAction(config_));
    }

    void Build() const override
    {
        this->SetUserAction(new RunAction(config_));
        this->SetUserAction(new PrimaryGeneratorAction(config_));
    }

  private:
    Config config_;
};


int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " [config.json]\n";
        return 1;
    }

    ROOT::EnableThreadSafety();

    auto config = Config::from_inp_file(argv[1]);

    std::unique_ptr<G4RunManager> run_manager(G4RunManagerFactory::CreateRunManager(config.num_threads == 1 ? G4RunManagerType::Serial : G4RunManagerType::MT));
    if (auto* rm_mt = dynamic_cast<G4MTRunManager*>(run_manager.get()))
    {
        rm_mt->SetNumberOfThreads(config.num_threads);
    }

    run_manager->SetUserInitialization(new DetectorConstruction(config));
    run_manager->SetUserInitialization(new FTFP_BERT(0));
    run_manager->SetUserInitialization(new ActionInitialization(config));

    run_manager->Initialize();

    run_manager->BeamOn(config.num_events);

    return 0;
}
