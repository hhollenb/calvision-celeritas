#pragma once

#include "TDirectory.h"
#include "TKey.h"
#include "TH1.h"

#include <vector>
#include <string>



double calc_average(std::vector<double> const& xs)
{
    double total = 0;
    for (double x : xs)
    {
        total += x;
    }
    return total / xs.size();
}



class ForeachDirectory
{
  public:
    ForeachDirectory(TDirectory* dir)
        : dir_(dir)
    {}

    template<class F>
    void operator()(F const& f) const
    {
        for (TObject* obj : *dir_->GetListOfKeys())
        {
            TKey* key = (TKey*) obj;
            if (TDirectory* d = dir_->Get<TDirectory>(key->GetName()))
            {
                f(d);
            }
        }
    }

  private:
    TDirectory* dir_;
};


inline std::vector<std::string> const& detector_names()
{
    static std::vector<std::string> detectors{"detector", "fsc_detector"};
    return detectors;
}

inline std::vector<std::string> const& process_names()
{
    static std::vector<std::string> processes{"cherenkov", "scintillation"};
    return processes;
}


inline double collect_event_hits(TDirectory* event_dir, std::string const& process_name)
{
    double total = 0;
    for (auto const& detector : detector_names())
    {
        TH1* hist = event_dir->Get<TH1>((detector + "_Celeritas_" + process_name + "_energy").c_str());
        total += hist->Integral();
    }
    return total;
}

inline std::vector<double> collect_event_totals(TDirectory* run_dir, std::string const& process_name)
{
    std::vector<double> totals;
    ForeachDirectory{run_dir}([&] (TDirectory* event_dir) { totals.push_back(collect_event_hits(event_dir, process_name)); });
    return totals;
}
