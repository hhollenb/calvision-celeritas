#include "EnergyDepositReader.hh"

#include "../EnergyDepositData.hh"

std::vector<celeritas::optical::GeneratorDistributionData>
EnergyDepositReader::operator()(std::string const& filename) const
{
    std::unique_ptr<TFile> file = std::make_unique<TFile>(filename.c_str());
    TTree* tree = (TTree*) file->Get("data");

    EdepData data;
    tree->SetBranchAddress("type", &data.type);
    tree->SetBranchAddress("num_photons", &data.num_photons);
    tree->SetBranchAddress("primary", &data.primary);
    tree->SetBranchAddress("step_length", &data.step_length);
    tree->SetBranchAddress("charge", &data.charge);
    tree->SetBranchAddress("material", &data.material);
    tree->SetBranchAddress("continuous_edep_fraction", &data.continuous_edep_fraction);

    tree->SetBranchAddress("pre_speed", &data.pre_step.speed);
    tree->SetBranchAddress("pre_time", &data.pre_step.time);
    tree->SetBranchAddress("pre_pos_x", &data.pre_step.pos_x);
    tree->SetBranchAddress("pre_pos_y", &data.pre_step.pos_y);
    tree->SetBranchAddress("pre_pos_z", &data.pre_step.pos_z);

    tree->SetBranchAddress("post_speed", &data.post_step.speed);
    tree->SetBranchAddress("post_time", &data.post_step.time);
    tree->SetBranchAddress("post_pos_x", &data.post_step.pos_x);
    tree->SetBranchAddress("post_pos_y", &data.post_step.pos_y);
    tree->SetBranchAddress("post_pos_z", &data.post_step.pos_z);

    std::vector<celeritas::optical::GeneratorDistributionData> results;
    results.reserve(tree->GetEntries());

    for (unsigned int i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);
        results.push_back(data.to_celer());
    }

    return results;
}
