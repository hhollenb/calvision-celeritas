#include "parser.hh"

#include "TFile.h"
#include "TCanvas.h"
#include "TParameter.h"

void do_plot(TH1D* hist, TDirectory* run_dir)
{        
    auto canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);
    hist->Draw("HIST");
    canvas->Draw();
    canvas->SaveAs((std::string(hist->GetName()) + "-" + std::string(run_dir->GetName()) + ".pdf").c_str());
}

void plot_leakage(TDirectory* run_dir)
{
    auto lateral_leakage = std::make_unique<TH1D>("lateral_leakage", "Lateral Shower Leakage;E_{leak}/E_{primary};Num Events", 1000, 0, 1.0);
    auto longitudinal_leakage = std::make_unique<TH1D>("longitudinal_leakage", "Longitudinal Shower Leakage;E_{leak}/E_{primary};Num Events", 100, 0, 1.0);

    auto lateral_count = std::make_unique<TH1D>("lateral_count", "Lateral Shower Leakage Count;Num Particles;Num Events", 40, 0, 40);
    auto longitudinal_count = std::make_unique<TH1D>("longitudinal_count", "Longitudinal Shower Leakage Count;Num Particles;Num Events", 100, 0, 200);

    ForeachDirectory{run_dir}([&] (TDirectory* event_dir) {
        lateral_leakage->Fill(event_dir->Get<TParameter<double>>("lateral_leakage")->GetVal());
        longitudinal_leakage->Fill(event_dir->Get<TParameter<double>>("longitudinal_leakage")->GetVal());
        lateral_count->Fill(event_dir->Get<TParameter<int>>("lateral_count")->GetVal());
        longitudinal_count->Fill(event_dir->Get<TParameter<int>>("longitudinal_count")->GetVal());
    });

    do_plot(lateral_leakage.get(), run_dir);
    do_plot(longitudinal_leakage.get(), run_dir);
    do_plot(lateral_count.get(), run_dir);
    do_plot(longitudinal_count.get(), run_dir);
}

void leakage()
{
    TFile* file = TFile::Open("output_electron.root");

    ForeachDirectory{file}(plot_leakage);
}
