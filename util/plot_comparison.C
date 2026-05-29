#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TRatioPlot.h"
#include "TKey.h"

#include <string>
#include <memory>

void plot_comparison()
{
    unsigned int event_num = 0;

    TFile* g4_file = TFile::Open(("geant_output_" + std::to_string(event_num) + ".root").c_str());
    TFile* cel_file = TFile::Open(("celercpu_output_" + std::to_string(event_num) + ".root").c_str());

    for (TObject* obj : *g4_file->GetListOfKeys())
    {
        TKey* key = (TKey*) obj;

        std::unique_ptr<TCanvas> canvas = std::make_unique<TCanvas>("canvas", "canvas", 800, 800);

        TH1* g4_hist = (TH1*) g4_file->Get(key->GetName());
        TH1* cel_hist = (TH1*) cel_file->Get(key->GetName());

        if (g4_hist && cel_hist)
        {
            if (g4_hist->GetDimension() == 1)
            {
                TRatioPlot* ratio_hist = new TRatioPlot(g4_hist, cel_hist);
                ratio_hist->Draw();

                canvas->Draw();
                canvas->SaveAs(("comparison_" + std::to_string(event_num) + "_" + key->GetName() + ".pdf").c_str());
            }
            else
            {
                g4_hist->Draw();
                canvas->Draw();
                canvas->SaveAs(("geant_" + std::to_string(event_num) + "_" + key->GetName() + ".pdf").c_str());
                canvas->Clear();

                cel_hist->Draw();
                canvas->Draw();
                canvas->SaveAs(("celercpu_" + std::to_string(event_num) + "_" + key->GetName() + ".pdf").c_str());
            }
        }
    }
}
