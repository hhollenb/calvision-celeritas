#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TRatioPlot.h"
#include "TKey.h"

#include <string>
#include <memory>


int const g4_marker = 20;
auto const scintillation_color = kRed;
auto const cherenkov_color = kGreen;




void normalize(TH1* hist)
{
    double integral = hist->Integral();
    if (integral != 0)
    {
        hist->Scale(1.0 / integral);
    }
}



TH1* get_total_hist(TDirectory* event_dir, std::string const& postfix)
{
    std::vector<std::string> detector_names{"rsc_detector", "fsc_detector"};

    TH1* result = nullptr;
    for (auto const& det_name : detector_names)
    {
        TH1* hist = event_dir->Get<TH1>((det_name + "_" + postfix).c_str());

        if (!result)
        {
            result = hist;
        }
        else
        {
            result->Add(hist);
        }
    }

    return result;
}

void load_event_hists(std::map<std::string, TH1*>& histograms, TDirectory* event_dir)
{
    for (auto& [name, hist] : histograms)
    {
        auto* result = get_total_hist(event_dir, name);
        if (hist)
        {
            hist->Add(result);
        }
        else
        {
            hist = result;
        }
    }
}


void plot_energy(std::map<std::string, TH1*>& histograms)
{
    TH1* cherenkov_geant_energy = histograms.at("Geant4_cherenkov_energy");
    TH1* scintillation_geant_energy = histograms.at("Geant4_scintillation_energy");
    TH1* cherenkov_celeritas_energy = histograms.at("Celeritas_cherenkov_energy");
    TH1* scintillation_celeritas_energy = histograms.at("Celeritas_scintillation_energy");

    normalize(cherenkov_geant_energy);
    normalize(scintillation_geant_energy);
    normalize(cherenkov_celeritas_energy);
    normalize(scintillation_celeritas_energy);

    auto canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);
    auto legend = std::make_unique<TLegend>(0.6, 0.7, 0.9, 0.9);

    scintillation_celeritas_energy->SetTitle("Measured Wavelength Spectrum;Wavelength [nm];Arb. Units");

    scintillation_geant_energy->SetMarkerStyle(g4_marker);
    scintillation_geant_energy->SetMarkerColor(scintillation_color);
    scintillation_geant_energy->SetStats(false);

    cherenkov_geant_energy->SetMarkerStyle(g4_marker);
    cherenkov_geant_energy->SetMarkerColor(cherenkov_color);
    cherenkov_geant_energy->SetStats(false);

    cherenkov_celeritas_energy->SetLineColor(cherenkov_color);
    cherenkov_celeritas_energy->SetStats(false);

    scintillation_celeritas_energy->SetLineColor(scintillation_color);
    scintillation_celeritas_energy->SetStats(false);


    scintillation_celeritas_energy->Draw("HIST");
    cherenkov_celeritas_energy->Draw("HIST SAME");
    scintillation_geant_energy->Draw("P SAME");
    cherenkov_geant_energy->Draw("P SAME");

    legend->AddEntry(scintillation_geant_energy, "Geant4 Scintillation");
    legend->AddEntry(cherenkov_geant_energy, "Geant4 Cherenkov");
    legend->AddEntry(scintillation_celeritas_energy, "Celeritas Scintillation");
    legend->AddEntry(cherenkov_celeritas_energy, "Celeritas Cherenkov");
    legend->Draw();

    canvas->Draw();
    canvas->SaveAs("comparison-energy.png");
}

void plot_signal(std::map<std::string, TH1*>& histogram)
{
    TH1* cherenkov_geant_signal = histogram.at("Geant4_cherenkov_signal");
    TH1* scintillation_geant_signal = histogram.at("Geant4_scintillation_signal");
    TH1* cherenkov_celeritas_signal = histogram.at("Celeritas_cherenkov_signal");
    TH1* scintillation_celeritas_signal = histogram.at("Celeritas_scintillation_signal");

    auto canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);
    auto legend = std::make_unique<TLegend>(0.6, 0.7, 0.9, 0.9);

    cherenkov_celeritas_signal->SetTitle("Measured Energy/Time Signal;Time [ns];Energy [eV]");

    scintillation_geant_signal->SetMarkerStyle(g4_marker);
    scintillation_geant_signal->SetMarkerColor(scintillation_color);
    scintillation_geant_signal->SetStats(false);

    cherenkov_geant_signal->SetMarkerStyle(g4_marker);
    cherenkov_geant_signal->SetMarkerColor(cherenkov_color);
    cherenkov_geant_signal->SetStats(false);

    scintillation_celeritas_signal->SetLineColor(scintillation_color);
    scintillation_celeritas_signal->SetStats(false);

    cherenkov_celeritas_signal->SetLineColor(cherenkov_color);
    cherenkov_celeritas_signal->SetStats(false);

    cherenkov_celeritas_signal->Draw("HIST");
    scintillation_celeritas_signal->Draw("HIST SAME");
    cherenkov_geant_signal->Draw("P SAME");
    scintillation_geant_signal->Draw("P SAME");

    legend->AddEntry(scintillation_geant_signal, "Geant4 Scintillation");
    legend->AddEntry(cherenkov_geant_signal, "Geant4 Cherenkov");
    legend->AddEntry(scintillation_celeritas_signal, "Celeritas Scintillation");
    legend->AddEntry(cherenkov_celeritas_signal, "Celeritas Cherenkov");
    legend->Draw();

    canvas->Draw();
    canvas->SaveAs("comparison-signal.png");
}

void plot_prompt_signal(std::map<std::string, TH1*>& histogram)
{
    TH1* cherenkov_geant_prompt_signal = histogram.at("Geant4_cherenkov_prompt_signal");
    TH1* scintillation_geant_prompt_signal = histogram.at("Geant4_scintillation_prompt_signal");
    TH1* cherenkov_celeritas_prompt_signal = histogram.at("Celeritas_cherenkov_prompt_signal");
    TH1* scintillation_celeritas_prompt_signal = histogram.at("Celeritas_scintillation_prompt_signal");

    auto canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);
    auto legend = std::make_unique<TLegend>(0.6, 0.7, 0.9, 0.9);

    cherenkov_celeritas_prompt_signal->SetTitle("Measured Energy/Time Signal;Time [ns];Energy [eV]");

    scintillation_geant_prompt_signal->SetMarkerStyle(g4_marker);
    scintillation_geant_prompt_signal->SetMarkerColor(scintillation_color);
    scintillation_geant_prompt_signal->SetStats(false);

    cherenkov_geant_prompt_signal->SetMarkerStyle(g4_marker);
    cherenkov_geant_prompt_signal->SetMarkerColor(cherenkov_color);
    cherenkov_geant_prompt_signal->SetStats(false);

    scintillation_celeritas_prompt_signal->SetLineColor(scintillation_color);
    scintillation_celeritas_prompt_signal->SetStats(false);

    cherenkov_celeritas_prompt_signal->SetLineColor(cherenkov_color);
    cherenkov_celeritas_prompt_signal->SetStats(false);

    cherenkov_celeritas_prompt_signal->Draw("HIST");
    scintillation_celeritas_prompt_signal->Draw("HIST SAME");
    cherenkov_geant_prompt_signal->Draw("P SAME");
    scintillation_geant_prompt_signal->Draw("P SAME");

    legend->AddEntry(scintillation_geant_prompt_signal, "Geant4 Scintillation");
    legend->AddEntry(cherenkov_geant_prompt_signal, "Geant4 Cherenkov");
    legend->AddEntry(scintillation_celeritas_prompt_signal, "Celeritas Scintillation");
    legend->AddEntry(cherenkov_celeritas_prompt_signal, "Celeritas Cherenkov");
    legend->Draw();

    canvas->Draw();
    canvas->SaveAs("comparison-prompt_signal.png");
}

void plot_comparison()
{
    TFile* file = TFile::Open("output_electron.root");

    for (TObject* run_obj : *file->GetListOfKeys())
    {
        TKey* run_key = (TKey*) run_obj;
        TDirectory* run_dir = file->Get<TDirectory>(run_key->GetName());

        std::map<std::string, TH1*> histograms;
        histograms.emplace("Geant4_cherenkov_energy", nullptr);
        histograms.emplace("Geant4_scintillation_energy", nullptr);
        histograms.emplace("Celeritas_cherenkov_energy", nullptr);
        histograms.emplace("Celeritas_scintillation_energy", nullptr);
        histograms.emplace("Geant4_cherenkov_signal", nullptr);
        histograms.emplace("Geant4_scintillation_signal", nullptr);
        histograms.emplace("Celeritas_cherenkov_signal", nullptr);
        histograms.emplace("Celeritas_scintillation_signal", nullptr);
        histograms.emplace("Geant4_cherenkov_prompt_signal", nullptr);
        histograms.emplace("Geant4_scintillation_prompt_signal", nullptr);
        histograms.emplace("Celeritas_cherenkov_prompt_signal", nullptr);
        histograms.emplace("Celeritas_scintillation_prompt_signal", nullptr);


        double num_events = 0;
        for (TObject* event_obj : *run_dir->GetListOfKeys())
        {
            TKey* event_key = (TKey*) event_obj;
            if (TDirectory* event_dir = run_dir->Get<TDirectory>(event_key->GetName()))
            {
                num_events += 1;
                load_event_hists(histograms, event_dir);
            }
        }

        for (auto& [name, hist] : histograms)
        {
            hist->Scale(1.0 / num_events);
        }

        plot_energy(histograms);
        plot_signal(histograms);
        plot_prompt_signal(histograms);
    }
}
