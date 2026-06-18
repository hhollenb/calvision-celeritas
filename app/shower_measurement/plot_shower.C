#include "TFile.h"
#include "TCanvas.h"
#include "TH2.h"
#include "TH1.h"
#include "TTree.h"
#include "TLegend.h"

int const width = 1600;
int const height = 800;

void draw_hist(std::string const& name, TH1* hist)
{
    std::unique_ptr<TCanvas> canvas = std::make_unique<TCanvas>("canvas", "canvas", width, height);
    hist->Draw("HIST");
    canvas->Draw();
    canvas->SaveAs(name.c_str());
}

void draw_mix_energy(TH1* edep_hist, TH1* leakage_hist)
{
    std::unique_ptr<TCanvas> canvas = std::make_unique<TCanvas>("canvas", "canvas", width, height);
    std::unique_ptr<TLegend> legend = std::make_unique<TLegend>(0.6, 0.8, 0.9, 0.9);

    leakage_hist->SetTitle("Energy Distribution;Energy [GeV];Num Events");
    leakage_hist->SetLineColor(kRed);
    leakage_hist->SetStats(false);
    leakage_hist->Draw("HIST");

    edep_hist->SetLineColor(kBlack);
    edep_hist->SetStats(false);
    edep_hist->Draw("HIST SAME");

    legend->AddEntry(edep_hist, "Energy Deposits");
    legend->AddEntry(leakage_hist, "Leakage Energy");
    legend->Draw();

    canvas->Draw();
    canvas->SaveAs("edep_leakage_energy.png");
}


struct Histograms
{
    TH2D* xz_profile;
    TH2D* yz_profile;
    TH2D* xy_profile;
    double total_energy;

    Histograms(std::string const& postfix)
    {
        xz_profile = new TH2D(("xz_profile" + postfix).c_str(),
                              "XZ Profile;Z [cm];X [cm]",
                              100, -12, 12,
                              100, -7, 7);
        yz_profile = new TH2D(("yz_profile" + postfix).c_str(),
                              "YZ Profile;Z [cm];Y [cm]",
                              100, -12, 12,
                              100, -7, 7);
        xy_profile = new TH2D(("xy_profile" + postfix).c_str(),
                              "XY Profile;X [cm];Y [cm]",
                              100, -7, 7,
                              100, -7, 7);
    }

    Histograms()
        : Histograms("")
    {}

    Histograms(unsigned int event_num)
        : Histograms("_" + std::to_string(event_num))
    {}


    static Histograms load_event(unsigned int event_num, TFile* file)
    {
        Histograms hists(event_num);

        TTree* tree = file->Get<TTree>(("event_" + std::to_string(event_num)).c_str());
        
        double x_pos, y_pos, z_pos, energy;
        tree->SetBranchAddress("pos_x", &x_pos);
        tree->SetBranchAddress("pos_y", &y_pos);
        tree->SetBranchAddress("pos_z", &z_pos);
        tree->SetBranchAddress("edep", &energy);

        hists.total_energy = 0;

        for (int n = 0; n < tree->GetEntries(); n++)
        {
            tree->GetEntry(n);

            hists.total_energy += energy;

            hists.xz_profile->Fill(z_pos, x_pos, energy);
            hists.yz_profile->Fill(z_pos, y_pos, energy);
            hists.xy_profile->Fill(x_pos, y_pos, energy);
        }

        return hists;
    }

    void add(Histograms const& hists)
    {
        xz_profile->Add(hists.xz_profile);
        yz_profile->Add(hists.yz_profile);
        xy_profile->Add(hists.xy_profile);
    }

    void draw(std::string const& prefix)
    {
        draw_hist((prefix + "_xz_profile.png").c_str(), xz_profile);
        draw_hist((prefix + "_yz_profile.png").c_str(), yz_profile);
        draw_hist((prefix + "_xy_profile.png").c_str(), xy_profile);
    }
};

struct LeakageHistograms
{
    TH2D* xz_profile;
    TH2D* yz_profile;
    TH2D* xy_profile;
    double total_energy;

    LeakageHistograms(std::string const& postfix)
    {
        xz_profile = new TH2D(("xz_leakage_profile" + postfix).c_str(),
                              "XZ Profile;Z [cm];X [cm]",
                              100, -12, 12,
                              100, -7, 7);
        yz_profile = new TH2D(("yz_leakage_profile" + postfix).c_str(),
                              "YZ Profile;Z [cm];Y [cm]",
                              100, -12, 12,
                              100, -7, 7);
        xy_profile = new TH2D(("xy_leakage_profile" + postfix).c_str(),
                              "XY Profile;X [cm];Y [cm]",
                              100, -7, 7,
                              100, -7, 7);
    }

    explicit LeakageHistograms(bool is_lateral)
        : LeakageHistograms(is_lateral ? std::string("lateral") : std::string("longitudinal"))
    {}

    LeakageHistograms(unsigned int event_num, std::string const& form)
        : LeakageHistograms("_" + form + "_" + std::to_string(event_num))
    {}


    static LeakageHistograms load_event(unsigned int event_num, TFile* file, std::string const& form)
    {
        LeakageHistograms hists(event_num, form);

        std::string tree_name = "event_" + form + "_leak_" + std::to_string(event_num);

        TTree* tree = file->Get<TTree>(tree_name.c_str());
        
        double x_pos, y_pos, z_pos, energy;
        tree->SetBranchAddress("pos_x", &x_pos);
        tree->SetBranchAddress("pos_y", &y_pos);
        tree->SetBranchAddress("pos_z", &z_pos);
        tree->SetBranchAddress("edep", &energy);

        hists.total_energy = 0;

        for (int n = 0; n < tree->GetEntries(); n++)
        {
            tree->GetEntry(n);

            hists.total_energy += energy;

            hists.xz_profile->Fill(z_pos, x_pos, energy);
            hists.yz_profile->Fill(z_pos, y_pos, energy);
            hists.xy_profile->Fill(x_pos, y_pos, energy);
        }

        return hists;
    }

    void add(LeakageHistograms const& hists)
    {
        xz_profile->Add(hists.xz_profile);
        yz_profile->Add(hists.yz_profile);
        xy_profile->Add(hists.xy_profile);
    }

    void draw(std::string const& prefix)
    {
        draw_hist((prefix + "_xz_profile.png").c_str(), xz_profile);
        draw_hist((prefix + "_yz_profile.png").c_str(), yz_profile);
        draw_hist((prefix + "_xy_profile.png").c_str(), xy_profile);
    }
};


void plot_shower()
{
    std::string filename = "edep_result.root";

    auto* file = TFile::Open(filename.c_str());
    
    Histograms hists;
    LeakageHistograms leakage_lateral_hists{true};
    LeakageHistograms leakage_longitudinal_hists{false};

    TH1D* total_energy = new TH1D("total_energy", "Total Energy Deposited;Energy [GeV];Num Events", 200, 0, 11);
    TH1D* leakage_energy = new TH1D("leakage_energy", "Leakage Energy;Energy [GeV];Num Events", 200, 0, 11);
    TH1D* sum_energy = new TH1D("sum_energy", "Deposit + Leakage Energy;Energy [GeV];Num Events", 200, 7, 10);

    for (unsigned int i = 0; i < 1000; i++)
    {
        auto event_hists = Histograms::load_event(i, file);
        auto event_leakage_lateral_hists = LeakageHistograms::load_event(i, file, "lateral");
        auto event_leakage_longitudinal_hists = LeakageHistograms::load_event(i, file, "longitudinal");

        hists.add(event_hists);
        leakage_lateral_hists.add(event_leakage_lateral_hists);
        leakage_longitudinal_hists.add(event_leakage_longitudinal_hists);

        double total_leakage_energy = event_leakage_lateral_hists.total_energy + event_leakage_longitudinal_hists.total_energy;

        total_energy->Fill(event_hists.total_energy);
        leakage_energy->Fill(total_leakage_energy);
        sum_energy->Fill(event_hists.total_energy + total_leakage_energy);
    }

    hists.draw("edep");
    leakage_lateral_hists.draw("lateral");
    leakage_longitudinal_hists.draw("longitudinal");

    draw_hist("total_energy.png", total_energy);
    draw_hist("leakage_energy.png", leakage_energy);
    draw_hist("sum_energy.png", sum_energy);

    draw_mix_energy(total_energy, leakage_energy);
}
