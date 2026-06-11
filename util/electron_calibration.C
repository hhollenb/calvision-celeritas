#include "parser.hh"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TParameter.h"


void calibrate_run(TDirectory* run_dir)
{
    double energy = run_dir->Get<TParameter<double>>("primary_energy")->GetVal();

    std::vector<double> num_cherenkov = collect_event_totals(run_dir, "cherenkov");
    std::vector<double> num_scintillation = collect_event_totals(run_dir, "scintillation");


    double avg_cherenkov = calc_average(num_cherenkov);;
    double avg_scintillation = calc_average(num_scintillation);

    for (double& c : num_cherenkov)
    {
        c /= avg_cherenkov;
    }

    for (double& s : num_scintillation)
    {
        s /= avg_scintillation;
    }


    auto* canvas = new TCanvas("canvas", "canvas", 800, 800);
    auto frame = canvas->DrawFrame(0, 0, 1.2, 1.2, "Electron Light Yield");
    frame->GetXaxis()->SetTitle("Scintillation Yield");
    frame->GetYaxis()->SetTitle("Cherenkov Yield");

    TGraph* cvs = new TGraph(num_cherenkov.size(), num_scintillation.data(), num_cherenkov.data());
    cvs->SetMarkerStyle(20);
    cvs->SetMarkerSize(1.0);
    cvs->Draw("P");

    canvas->Draw();
    canvas->SaveAs(("electron-scatter-" + std::string(run_dir->GetName()) + ".pdf").c_str());

    std::cout << "kC = " << (avg_cherenkov / energy) << "\n";
    std::cout << "kS = " << (avg_scintillation / energy) << "\n";

    std::cout << "average C = " << avg_cherenkov << "\n";
    std::cout << "average S = " << avg_scintillation << "\n";
}



/**
 * Determine the DR calibration for a set of electron runs.
 */
void electron_calibration()
{
    TFile* file = TFile::Open("output_electron.root");

    ForeachDirectory{file}(calibrate_run);
}
