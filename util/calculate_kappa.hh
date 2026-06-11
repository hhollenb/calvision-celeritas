#include "parser.hh"

#include "TFile.h"
#include "TCanvas.h"
#include "TGraph.h"

double const kC = 518.098;
double const kS = 568.020;

double const electron_avg_cherenkov = 5180.98;
double const electron_avg_scintillation = 5680.20;


double calc_energy(double kappa, double C, double S)
{
    return (S / kS - kappa * C / kC) / (1.0 - kappa);
}


void calculate_run(TDirectory* run_dir)
{
    double energy = run_dir->Get<TParameter<double>>("primary_energy")->GetVal();

    std::vector<double> num_cherenkov = collect_event_totals(run_dir, "cherenkov");
    std::vector<double> num_scintillation = collect_event_totals(run_dir, "scintillation");

    for (double& c : num_cherenkov)
    {
        c /= electron_avg_cherenkov;
    }

    for (double& s : num_scintillation)
    {
        s /= electron_avg_scintillation;
    }

    TGraph* cvs = new TGraph(num_cherenkov.size(), num_scintillation.data(), num_cherenkov.data());
    cvs->Fit("pol1");

    TF1* linear_fit = cvs->GetFunction("pol1");
    double kappa = linear_fit->GetParameter(1);

    {
        auto canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);
        auto frame = canvas->DrawFrame(0, 0, 1.2, 1.2, "Pion-0 Light Yield");
        frame->GetXaxis()->SetTitle("Scintillation Yield");
        frame->GetYaxis()->SetTitle("Cherenkov Yield");

        cvs->SetMarkerStyle(20);
        cvs->SetMarkerSize(1.0);
        cvs->Draw("P");

        linear_fit->SetLineWidth(1);
        linear_fit->SetLineColor(kRed);
        linear_fit->Draw("same");

        canvas->Draw();
        canvas->SaveAs(("pion0-scatter-" + std::string(run_dir->GetName()) + ".pdf").c_str());
    }

    // std::cout << "kC = " << (avg_cherenkov / energy) << "\n";
    // std::cout << "kS = " << (avg_scintillation / energy) << "\n";

    // std::cout << "average C = " << avg_cherenkov << "\n";
    // std::cout << "average S = " << avg_scintillation << "\n";

    {
        TH1D* energy_hist = new TH1D("corrected_energies", "Corrected Pion-0 Energy;Measured / Actual;Num Events", 20, 0, 2);
        
        for (unsigned int i = 0; i < num_cherenkov.size(); i++)
        {
            double x = calc_energy(kappa, num_cherenkov[i], num_scintillation[i]) / energy;
            std::cout << x << "\n";
            energy_hist->Fill(x);
        }

        auto canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);
        energy_hist->Draw("HIST");
        canvas->Draw();
        canvas->SaveAs(("pion0-reco_energy-" + std::string(run_dir->GetName()) + ".pdf").c_str());
    }
}


void calculate_kappa()
{
    TFile* file = TFile::Open("output_pion0.root");
    ForeachDirectory{file}(calculate_run);
}
