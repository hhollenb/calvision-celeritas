#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"

#include <iostream>

void plot_edep_pos()
{
    std::cout << "rsc crystal half length: " << (7.3385 / 2) << "\n";

    TFile* file = TFile::Open("edep_0.root");
    TTree* tree = (TTree*) file->Get("data");

    TH1D* hist_pre_pos_z = new TH1D("hist_pos_z", "Pre-Step Z position;Z [cm];Num Photons", 100, -5, 5);
    TH1D* hist_post_pos_z = new TH1D("hist_post_z", "Post-Step Z position;Z [cm];Num Photons", 100, -5, 5);

    double pre_pos_z;
    double post_pos_z;

    tree->SetBranchAddress("pre_pos_z", &pre_pos_z);
    tree->SetBranchAddress("post_pos_z", &post_pos_z);

    for (unsigned int i = 0; i < tree->GetEntries(); i++)
    {
        tree->GetEntry(i);

        hist_pre_pos_z->Fill(pre_pos_z);
        hist_post_pos_z->Fill(post_pos_z);
    }

    TCanvas* canvas = new TCanvas("canvas", "canvas", 800, 800);
    canvas->Divide(1, 2);

    auto* pad = canvas->cd(1);
    hist_pre_pos_z->Draw("HIST");

    pad = canvas->cd(2);
    hist_post_pos_z->Draw("HIST");

    canvas->Draw();
    canvas->SaveAs("edep_pos_z.pdf");
}
