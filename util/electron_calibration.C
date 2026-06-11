#include "TFile.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TParameter.h"
#include "TKey.h"
#include "TDictionary.h"

// struct EventSignal
// {
//     unsigned int num_cherenkov_front;
//     unsigned int num_cherenkov_rear;
//     unsigned int num_scintillation_front;
//     unsigned int num_scintillation_rear;
// 
//     unsigned int num_cherenkov_total() const
//     {
//         return num_cherenkov_front + num_cherenkov_rear;
//     }
// 
//     unsigned int num_scintillation_total() const
//     {
//         return num_scintillation_front + num_scintillation_rear;
//     }
// };
// 
// struct CalibrationParameters
// {
//     double scale_cherenkov;      // kC
//     double scale_scintillation;  // kS
// };
// 
// /**
//  * Determine the kC and kS ratios for a set of monoenergtic electron events.
//  *
//  * For electron showers, the EM fraction is expected to be approximately f=1.
//  * The Cherenkov and scintillation scale factors are then given by
//  *      kC = C / E
//  *      kS = S / E
//  * where C and S are the total yields of Cherenkov and scintillation light
//  * in the detector.
//  */
// CalibrationParameters calibrate_energy(double energy, std::vector<EventSignal> const& events)
// {
//     unsigned int total_cherenkov = 0;
//     unsigned int total_scintillation = 0;
// 
//     for (auto const& event : events)
//     {
//         total_cherenkov += event.num_cherenkov_total();
//         total_scintillation += event.num_scintillation_total();
//     }
// 
//     CalibrationParameters result;
//     result.scale_cherenkov = static_cast<double>(total_cherenkov) / (events.size() * energy);
//     result.scale_scintillation = static_cast<double>(total_scintillation) / (events.size() * energy);
//     return result;
// }


double calc_average(std::vector<double> const& xs)
{
    double total = 0;
    for (double x : xs)
    {
        total += x;
    }
    return total / xs.size();
}



void calibrate_run(TDirectory* run_dir)
{
    std::vector<std::string> detectors{"detector", "fsc_detector"};

    double energy = run_dir->Get<TParameter<double>>("primary_energy")->GetVal();

    std::vector<double> num_cherenkov;
    std::vector<double> num_scintillation;

    for (TObject* event_obj : *run_dir->GetListOfKeys())
    {
        TKey* event_key = (TKey*) event_obj;
        if (TDirectory* event_dir = run_dir->Get<TDirectory>(event_key->GetName()))
        {
            double total_cherenkov = 0;
            double total_scintillation = 0;

            for (auto const& detector : detectors)
            {
                TH1* cherenkov_hist = event_dir->Get<TH1>((detector + "_Celeritas_cherenkov_energy").c_str());
                total_cherenkov += cherenkov_hist->Integral();

                TH1* scintillation_hist = event_dir->Get<TH1>((detector + "_Celeritas_scintillation_energy").c_str());
                total_scintillation += scintillation_hist->Integral();
            }

            num_cherenkov.push_back(total_cherenkov);
            num_scintillation.push_back(total_scintillation);
        }
    }


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
}



/**
 * Determine the DR calibration for a set of electron runs.
 */
void electron_calibration()
{
    TFile* file = TFile::Open("output_electron.root");

    for (TObject* run_obj : *file->GetListOfKeys())
    {
        TKey* run_key = (TKey*) run_obj;
        TDirectory* run_dir = file->Get<TDirectory>(run_key->GetName());

        calibrate_run(run_dir);
    }
}
