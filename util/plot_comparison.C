#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TRatioPlot.h"
#include "TKey.h"

#include <string>
#include <memory>

void make_event_comparisons(std::string const& run_name, std::string const& event_name, TDirectory* event_dir)
{
    std::vector<std::string> detector_names{"detector", "fsc_detector"};
    std::vector<std::string> gen_types{"cherenkov", "scintillation"};
    std::vector<std::string> hist_names{"energy", "time", "signal"};

    std::string file_prefix = "comparison-" + run_name + "-" + event_name;

    for (auto const& detector_name : detector_names)
    {
        for (auto const& gen_type : gen_types)
        {
            for (auto const& hist_name : hist_names)
            {
                std::unique_ptr<TCanvas> canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);

                TH1* g4_hist = event_dir->Get<TH1>((detector_name + "_Geant4_" + gen_type + "_" + hist_name).c_str());
                TH1* cel_hist = event_dir->Get<TH1>((detector_name + "_Celeritas_" + gen_type + "_" + hist_name).c_str());

                TRatioPlot* ratio_hist = new TRatioPlot(g4_hist, cel_hist);
                ratio_hist->Draw();

                canvas->Draw();
                canvas->SaveAs((file_prefix + "-" + detector_name + "-" + gen_type + "-" + hist_name + ".pdf").c_str());
            }
        }
    }
}


void plot_comparison()
{
    TFile* file = TFile::Open("output_pion0.root");

    for (TObject* run_obj : *file->GetListOfKeys())
    {
        TKey* run_key = (TKey*) run_obj;
        TDirectory* run_dir = file->Get<TDirectory>(run_key->GetName());

        for (TObject* event_obj : *run_dir->GetListOfKeys())
        {
            TKey* event_key = (TKey*) event_obj;
            if (TDirectory* event_dir = run_dir->Get<TDirectory>(event_key->GetName()))
            {
                make_event_comparisons(run_key->GetName(), event_key->GetName(), event_dir);
            }
        }
    }
}




// void plot_comparison()
// {
//     unsigned int event_num = 0;
// 
//     TFile* g4_file = TFile::Open(("geant_output_" + std::to_string(event_num) + ".root").c_str());
//     TFile* cel_file = TFile::Open(("celercpu_output_" + std::to_string(event_num) + ".root").c_str());
// 
//     for (TObject* obj : *g4_file->GetListOfKeys())
//     {
//         TKey* key = (TKey*) obj;
// 
//         std::unique_ptr<TCanvas> canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);
// 
//         TH1* g4_hist = (TH1*) g4_file->Get(key->GetName());
//         TH1* cel_hist = (TH1*) cel_file->Get(key->GetName());
// 
//         if (g4_hist && cel_hist)
//         {
//             if (g4_hist->GetDimension() == 1)
//             {
//                 TRatioPlot* ratio_hist = new TRatioPlot(g4_hist, cel_hist);
//                 ratio_hist->Draw();
// 
//                 canvas->Draw();
//                 canvas->SaveAs(("comparison_" + std::to_string(event_num) + "_" + key->GetName() + ".pdf").c_str());
//             }
//             else
//             {
//                 g4_hist->Draw();
//                 canvas->Draw();
//                 canvas->SaveAs(("geant_" + std::to_string(event_num) + "_" + key->GetName() + ".pdf").c_str());
//                 canvas->Clear();
// 
//                 cel_hist->Draw();
//                 canvas->Draw();
//                 canvas->SaveAs(("celercpu_" + std::to_string(event_num) + "_" + key->GetName() + ".pdf").c_str());
//             }
//         }
//     }
// }
