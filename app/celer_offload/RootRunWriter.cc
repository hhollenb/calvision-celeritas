#include "RootRunWriter.hh"

#include "G4ios.hh"

RootRunWriter::RootRunWriter(std::string const& filename,
                             std::map<std::string, std::string> const& detectors)
    : detectors_(detectors)
    , run_num_(0)
{
    G4cout << "Opening root file " << filename << G4endl;

    file_ = std::make_unique<TFile>(filename.c_str(), "RECREATE", "Set of runs");

    for (auto const& dets : detectors_)
    {
        buffer_.emplace(dets.first, DetectorScoring());
    }
}

RootRunWriter::~RootRunWriter()
{
    G4cout << "Closing file..." << G4endl;
    this->close();
}

void RootRunWriter::start_run(double energy)
{
    energies_.push_back(energy);

    run_tree_ = new TTree(("Run_" + std::to_string(run_num_)).c_str(), "Total hit readouts");
    run_tree_->SetDirectory(file_.get());
    for (auto const& dets : detectors_)
    {
        std::string const& det_name = dets.first;
        std::string const& branch_name = dets.second;

        run_tree_->Branch((branch_name + "_cherenkov").c_str(), &buffer_.at(det_name).num_cherenkov);
        run_tree_->Branch((branch_name + "_scintillation").c_str(), &buffer_.at(det_name).num_scintillation);
    }
}

void RootRunWriter::add_event_score(EventScores const& scores)
{
    for (auto const& s : scores)
    {
        DetectorScoring& b = buffer_.at(s.first);
        b.num_cherenkov = s.second.num_cherenkov;
        b.num_scintillation = s.second.num_scintillation;
    }

    run_tree_->Fill();
}

void RootRunWriter::end_run()
{
    run_tree_->Write();
    ++run_num_;
}

void RootRunWriter::close()
{
    if (file_)
    {
        file_->WriteObject(&energies_, "energies");
        file_->Write();
        file_->Close();
        file_.reset();
    }
}
