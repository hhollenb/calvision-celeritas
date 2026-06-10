#include "RootSignalWriter.hh"

RunSignalWriter::RunSignalWriter(TFile* file, unsigned int run_num, double primary_energy)
{
    file_ = std::make_unique<TDirectoryFile>(("run_" + std::to_string(run_num)).c_str(), "Run Data", "", file);
    energy_ = new TParameter<double>("primary_energy", primary_energy);
}

RunSignalWriter::~RunSignalWriter()
{
    this->close();
}

void RunSignalWriter::operator()(unsigned int event_id, EventHistograms const& hists)
{
    auto event_dir = std::make_unique<TDirectoryFile>(("event_" + std::to_string(event_id)).c_str(), "Event Data", "", file_.get());
    event_dir->cd();

    for (auto const& [_, h] : hists)
    {
        h.cherenkov.write(event_dir.get());
        h.scintillation.write(event_dir.get());
    }

    // event_dir->Write();
    event_dir->Close();
}

void RunSignalWriter::close()
{
    if (file_)
    {
        energy_->Write();
        file_->Write();
        file_->Close();
        file_.reset();
    }
}

RootSignalWriter::RootSignalWriter(std::string const& filename)
    : run_num_(0)
{
    file_ = std::make_unique<TFile>(filename.c_str(), "RECREATE", "Set of runs");
}

RootSignalWriter::~RootSignalWriter()
{
    this->close();
}

RunSignalWriter RootSignalWriter::next_run(double primary_energy)
{
    file_->cd();
    return RunSignalWriter(file_.get(), run_num_++, primary_energy);
}

void RootSignalWriter::close()
{
    if (file_)
    {
        file_->cd();
        file_->Write();
        file_->Close();
        file_.reset();
    }
}
