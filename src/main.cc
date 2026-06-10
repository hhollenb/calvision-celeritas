#include <iostream>

#include "celer/CelerRunner.hh"
#include "g4/GeantRunner.hh"

#include "TH1.h"


void print_usage(char* argv0)
{
    std::cout << "Usage: " << argv0 << " [runner] [problem]\n";
}


int main(int argc, char** argv)
{
    TH1::AddDirectory(kFALSE);

    if (argc != 2)
    {
        print_usage(argv[0]);
        return 1;
    }

    std::string runner_name(argv[1]);

    if (runner_name == "geant4")
    {
        GeantRunner::Options opts;
        opts.track_photons = false;

        GeantRunner runner{opts};

        runner();
    }
    else if (runner_name == "celer-cpu")
    {
        CelerRunner runner;

        runner();
    }
    else if (runner_name == "celer-gpu")
    {
    }
    else
    {
        std::cout << "Invalid runner argument: " << runner_name << "\n";
    }

    return 0;
}
