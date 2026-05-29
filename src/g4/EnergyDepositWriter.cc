#include "EnergyDepositWriter.hh"

EnergyDepositWriter::EnergyDepositWriter()
{}

EnergyDepositWriter::~EnergyDepositWriter()
{
    this->close();
}

void EnergyDepositWriter::operator()(celeritas::optical::GeneratorDistributionData const& data)
{
    buffer_ = data;
    tree_->Fill();
}

void EnergyDepositWriter::open(std::string const& filename)
{
    std::cout << "Opening file " << filename << "\n";
    file_ = std::make_unique<TFile>(filename.c_str(), "RECREATE", "Energy deposits");
    // Tree owned by file
    tree_ = new TTree("data", "Energy deposits");
    tree_->SetDirectory(file_.get());

    tree_->Branch("type", &buffer_.type);
    tree_->Branch("num_photons", &buffer_.num_photons);
    tree_->Branch("primary", &buffer_.primary);
    tree_->Branch("step_length", &buffer_.step_length);
    tree_->Branch("charge", &buffer_.charge);
    tree_->Branch("material", &buffer_.material);
    tree_->Branch("continuous_edep_fraction", &buffer_.continuous_edep_fraction);

    tree_->Branch("pre_speed", &buffer_.pre_step.speed);
    tree_->Branch("pre_time", &buffer_.pre_step.time);
    tree_->Branch("pre_pos_x", &buffer_.pre_step.pos_x);
    tree_->Branch("pre_pos_y", &buffer_.pre_step.pos_y);
    tree_->Branch("pre_pos_z", &buffer_.pre_step.pos_z);

    tree_->Branch("post_speed", &buffer_.post_step.speed);
    tree_->Branch("post_time", &buffer_.post_step.time);
    tree_->Branch("post_pos_x", &buffer_.post_step.pos_x);
    tree_->Branch("post_pos_y", &buffer_.post_step.pos_y);
    tree_->Branch("post_pos_z", &buffer_.post_step.pos_z);

    std::cout << "Finished opening file\n";
}

void EnergyDepositWriter::close()
{
    std::cout << "Closing tree...\n";
    if (tree_)
    {
        tree_->SetDirectory(file_.get());
        // tree_->Write();
    }

    if (file_)
    {
        file_->Write();
        file_->Close();
    }

    // tree will be deleted by the file
    tree_ = nullptr;
    file_.reset();
    std::cout << "Done.\n";
}
